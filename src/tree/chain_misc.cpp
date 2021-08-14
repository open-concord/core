#include <nlohmann/json.hpp>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>

#include "../../inc/crypt++.h"
#include "../../inc/b64.h"
#include "../../inc/tree.h"

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

std::string chain_encrypt(json data, std::string dsa_pri_key, std::string rsa_pub_key, std::string aes_key, char mt) {
    std::string enc_string = data.dump();
    if (mt != 'p') { //no encryption for dec
        std::string rsa_pub_key;
        
        enc_string = b64_encode(lock_msg(enc_string, (mt == 'p'), b64_decode(dsa_pri_key), b64_decode(aes_key), b64_decode(rsa_pub_key)));
    }
    return enc_string;
}