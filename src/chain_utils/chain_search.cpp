#include <nlohmann/json.hpp>
#include <array>
#include <map>
#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <boost/function.hpp>
#include <boost/bind/bind.hpp>

#include "../../inc/crypt++.h"
#include "../../inc/chain_utils.h"
#include "../../inc/b64.h"

using namespace boost::placeholders;

std::map<char, std::vector<std::string>> qtypes {
    {'s', {"set"}},
    {'r', {"crole", "drole", "grole", "nserv"}},
    {'m', {"inv", "rem", "nserv"}},
    {'p', {"msg", "img", "li", "imgli", "vidli"}}
};

json get_continuity_value(std::vector<json> defs, std::string key) {
    for (auto def : defs) {
        if (def.find(key) != def.end()) return def[key];
    }
    throw std::out_of_range("No such value defined");
}

bool no_filter(json data) {
    return true;
}

bool type_filter(char qtype, json data) {
    std::vector<std::string>& type_tags = qtypes[qtype];
    return (std::find(type_tags.begin(), type_tags.end(), std::string(data["t"])) != type_tags.end());
}

void apply_member_change(std::map<std::string, std::string>& m_keys, json m_data) {
    if (m_data["t"] == "inv" || m_data["t"] == "nserv") m_keys[m_data["d"]["nm"]] = m_data["d"]["pubk"]; //add member key
    else if (m_data["t"] == "rem") m_keys[m_data["d"]["nm"]] = ""; //remove member key
}

std::vector<json> chain_search(std::vector<std::vector<std::string>> chain, char message_type, std::string target_trip, std::string key, boost::function<bool(json)> filter, int start_b, int end_b) {
    //note: depending on message_type, key can be an AES key or PEM RSA key
    
    //member searches are special because they need to be done back-front and with unlimited range
    std::map<std::string, std::string> member_sig_keys;
    bool member_search = false;
    if (message_type == 'm') {
        message_type = 's'; //use server procedure
        member_search = true;
        //member searches need to have set characteristics
        filter = boost::bind(type_filter, 'm', _1);
        start_b = -1;
        end_b = -1;
    } else if (message_type == 's') { //not a member search, but still a server search
        std::vector<json> member_data = chain_search(chain, 'm', target_trip, key);
        for (auto member_change : member_data) apply_member_change(member_sig_keys, member_change); //load sig keys from a member search
    }
    size_t valid_block_iter = 0;
    std::vector<json> outputs;
    std::string dsa_pubkey;
    for (size_t i = 0; i < chain.size(); i++) {
        size_t ri = (member_search ? i : chain.size() - 1 - i);
        auto block = chain[ri];
        json cont;
        if (block[5] != target_trip) continue;
        if (message_type == 'd') {
            json data;
            try {
                data = json::parse(block[6]); //get overall data
                cont = json::parse(std::string(data["cont"])); //get declaration content
                std::string pubkey = cont["pubk"]; //get pubkey from content
                if (!filter(cont)) continue;
                if (!DSA_verify(pubkey, b64_decode(data["sig"]), data["cont"])) continue; //not properly signed -> skip
            }
            catch(int e) {
                continue; //error if not plaintext json, with cont json and pubkey field -> skip
            }
        }
        else {
            try {
                std::string skey = b64_decode(key);
                std::array<std::string, 2> unlocked = unlock_msg(b64_decode(block[6]), (message_type == 'p'), skey, skey);
                cont = json::parse(unlocked[0]);
                if (!filter(cont)) continue; //needs to pass filter
                if (member_search && outputs.size() == 0) {
                    assert(cont["t"] == "nserv"); //first member search block needs to be server initialization
                    member_sig_keys[cont["d"]["nm"]] = cont["d"]["pubk"]; //server creator is automatically a member
                }
                if (!DSA_verify(b64_decode(member_sig_keys[cont["s"]]), unlocked[1], unlocked[0])) continue; //verify with sender sig, per known member sigs. This will error if sender sig is unknown (i.e. sender is not a member).
                if (member_search) apply_member_change(member_sig_keys, cont);
            }
            catch(int e) {
                continue;
            }
        }
        //if we got this far, the block is a decent one.
        valid_block_iter++;
        if ((start_b != -1) && (valid_block_iter < start_b)) continue; //needs to be at least at start of range
        if ((end_b != -1) && (valid_block_iter > end_b)) break; //can be at most end of range
        outputs.push_back(cont);
    }
    return outputs;
}