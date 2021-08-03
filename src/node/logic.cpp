#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <functional>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "../../inc/node.h"
#include "../../inc/chain_utils.h"
#include "../../inc/b64.h"
#include "../../inc/crypt++.h"
#include "../../inc/tree.h"
#include "../../inc/rw.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/** this file only holds the message logic
 * which will be sure to grow exponentially
 */

// temp var for cfg path
//RW rw_handler;
//json cfg = json::parse(rw_handler.read("../../cfg/main.json"));

//addition
json addition(Conn* conn, json cont) {
    auto KEYS  = (conn->message_context).user_keys_map[cont["u"]];
    auto TREE = (*(conn->parent_chains))[cont["ch"]];
    std::string rsa_pub_key = "";
    if (cont["mt"] == "p") rsa_pub_key = get_continuity_value(chain_search(TREE.get_chain(), 'p', cont["s"], ""), "rsa_pubk");
    TREE.generate_branch(false, 
        chain_encrypt(cont, KEYS.dsa_pri_key, rsa_pub_key, KEYS.server_keys[cont["s"]], std::string(cont["mt"]).at(0)), 
        cont["s"]
    );
    return {
        {"success", 1}
    };
}

// queries
json query (Conn* conn, json cont) {
    auto KEYS = (conn->message_context).user_keys_map[cont["u"]];
    auto TREE = (*(conn->parent_chains))[cont["ch"]];

    std::vector<std::vector<std::string>> blocks = TREE.get_chain();
    char search_type_char = std::string(cont["mt"]).at(0);
    if (cont["imt"] == "m") search_type_char = 'm';
    std::vector<json> search_results = chain_search(blocks, search_type_char, cont["s"], KEYS.server_keys[cont["s"]], boost::bind(type_filter, (char) std::string(cont["imt"]).at(0), _1), cont["r"][0], cont["r"][1]);
    return search_results;
}

json key_change(Conn* conn, json cont) {
    auto KEYS = (conn->message_context).user_keys_map[cont["u"]];
    if (cont.find("servkeys") != cont.end()) {
        for (auto servkey : cont["servkeys"]) {
            KEYS.server_keys[servkey["s"]] = servkey["k"];
        }
    }
    if (cont.find("sigkey") != cont.end()) {
        KEYS.dsa_pri_key = cont["sigkey"];
    }
    if (cont.find("enckey") != cont.end()) {
        KEYS.dsa_pri_key = cont["enckey"];
    }
    return {
        {"success", 1}
    };
}

// keygen
json key_gen(Conn* conn, json cont) {
    // index 0 is pri, index 1 is pub
    std::array<std::string, 2> keys;
    switch (((std::string) cont["kt"]).at(0)) {
        case 'D': // DSA
            keys = DSA_keygen();
            break;
        case 'R': // RSA
            keys = RSA_keygen();
            break;
        case 'A': // AES
            // 256 byte key (maybe set this up to be cfg?)
            keys[0] = AES_keygen();
            keys[1] = "";
            break;
        default:
            throw;
    };
    json retc = {
        "pri", b64_encode(keys[0]),
        "pub", b64_encode(keys[1]),
        "kt", cont["kt"]
    };
    return retc;
}

json encdec(Conn* conn, json cont) {
    json retc;
    if (cont["dec"]) {
        retc["plain"] = AES_decrypt(b64_decode(cont["aes_key"]), b64_decode(cont["nonce"]), b64_decode(cont["cipher"]));
    } else {
        std::array<std::string, 2> results = AES_encrypt(b64_decode(cont["aes_key"]), cont["plain"]);
        retc["cipher"] = b64_encode(results[0]);
        retc["nonce"] = b64_encode(results[1]);
    }
    return retc;
}

void update_chain(Conn *conn) {
    auto CTX = (conn->message_context);
    auto TREE = (*(conn->parent_chains))[CTX.chain_trip];
    std::vector<json> forward_wchain;
    for (size_t i = 0; i < CTX.wchain.size(); i++)
        TREE.chain_push(
            CTX.wchain[CTX.wchain.size() - i]
        );
    json jret = {
        {"err", 0},
        {"t", "nb"},
        {"c", {
            {"ch", CTX.chain_trip},
            {"bc", CTX.wchain.size()}
        }}
    };
    (*(conn->parent_local_conn))->send(jret.dump());
}

// error handler
json error(int error_code) {
    json ret = {
        {"FLAG", "ERROR"},
        {"CONTENT", error_code}
    };
    return ret;
}

// - handle functions -

json send_blocks(Conn *conn, json args) {
    try {
        auto CTX = (conn->message_context);
        auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

        json ret = {
            {"FLAG", "BLOCKS"}
        };
        std::vector<json> blocks;
        for (size_t backi = 0; backi < CTX.k; backi++) {
            size_t workingi = CTX.lastbi - 1 - backi;
            blocks.push_back({
                {"#", workingi},
                {"b", TREE.get_chain()[workingi]}
            });
            if (workingi == 0) {
                break;
            }
        }
        if (CTX.lastbi > CTX.k) CTX.lastbi = CTX.lastbi - CTX.k;
        else CTX.lastbi = 0;
        ret["CONTENT"] = {{"blocks", blocks}};
        return ret;
    } catch (int err) {
        return error(err);
    }
}

json begin_sending_blocks(Conn *conn, json cont) {
    try {
        auto CTX = (conn->message_context);
        auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

        CTX.chain_trip = cont["chain"];
        CTX.lastbi = TREE.get_chain().size();
        CTX.k = cont["k"];
        CTX.pow_min = json(TREE.get_chain()[0][5])["p"];
        return send_blocks(conn, cont);
    } catch (int err) {
        return error(err);
    }
}

json evaluate_blocks(Conn *conn, json cont) {
    try {
        auto CTX = (conn->message_context);
        auto TREE = (*(conn->parent_chains))[CTX.chain_trip];
        // check each subsequent block, see contact.txt
        json ret = {
        {"FLAG", "ABSENT/V"}
        };
        for (auto block : cont["CONTENT"]["blocks"]) {
            size_t bi = block["#"];
            ret["CONTENT"]["i"] = bi;
            if (CTX.wchain.size() > 0 &&
                !(verify_block(block, CTX.pow_min)
                    && block["b"][2] == CTX.wchain.back()[1]
                )
            ) {
                ret["FLAG"] == "ABSENT/NV";
                break;
            }
            if (bi < TREE.get_chain().size()) {
                bool all_equal = true;
                for (size_t i = 0; i < 6; i++) all_equal = (all_equal && TREE.get_chain()[bi][i] == block["b"][i]);
                if (all_equal) {
                    if (bi == TREE.get_chain().size() - 1) ret["FLAG"] = "PRESENT/T";
                    else ret["FLAG"] == "PRESENT/NT";
                    break;
                }
            }
            CTX.wchain.push_back(block["b"].get<std::vector<std::string>>());
        }
        if (ret["FLAG"] == "PRESENT/T" || (ret["FLAG"] == "ABSENT/V" && ret["CONTENT"]["i"] == 0)) update_chain(conn);
        return ret;
    } catch (int err) {
        return error(err);
    }
}
// - end of C2C handle functions -

// there's only one standard request for UI2C
json handle_request(Conn* conn, json cont) {
    try {
        json ret;
        ret["t"] = cont["t"];
        switch (((std::string) cont["t"]).at(0)) {
            case 'a': // addition (decleration, intraserver)
                ret["c"] = addition(conn, cont);
                break;
            case 'q': // query (messages)
                ret["c"] = query(conn, cont);
                break;
            case 'c': // user-specific data changes (keys)
                ret["c"] = key_change(conn, cont);
                break;
            case 'g': // keygen
                ret["c"] = key_gen(conn, cont);
                break;
            case 'e':
                ret["c"] = encdec(conn, cont);
                break;
            default: // none of the actual flags were present, throw error
                throw;
        };
        return ret;
    } catch (std::exception& err) {
        std::cout << err.what() << "\n";
    }
}

// map of communication roadmap
std::map<std::string /*prev flag*/, json (*)(Conn*, json)> next {
    {"READY", &begin_sending_blocks},
    {"BLOCKS", &evaluate_blocks},
    {"ABSENT/V", &send_blocks}
};

std::string message_logic(Conn *conn) {
    // make sure to;
    // add the socket's info to khosts
    json parsed = json::parse(conn->incoming_msg);
    std::cout << "NEW MSG: " << parsed << "\n";

    // message parsing
    std::string cmd = parsed["FLAG"];
    json cont = parsed["CONTENT"];

    // temp return var
    std::string rmsg;

    // client and server roles can both be stored in func map; communication flags ensure proper order of execution
    try {
        if (!conn->local) {
            rmsg = ((*next[cmd])(conn, cont)).dump();
        } else {rmsg = handle_request(conn, cont).dump();}
    } catch (int err) {
        rmsg = error(err).dump();
    }

    // change 'done' to true to end the communication (make sure to return a <close> message)
    // conn_obj->done = true;
    // clean incoming_message for clean recursion
    conn->incoming_msg.clear();
    // return response
    return rmsg;
}