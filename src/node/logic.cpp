#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <functional>

#include "../../inc/node.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/** this file only holds the message logic
 * which will be sure to grow exponentially
 */

void update_chain(Conn *conn) {
    #define CTX (conn->message_context)
    #define TREES (*(conn->parent_chains))
    std::vector<std::vector<std::string>> reversed = CTX.wchain;
    std::reverse(reversed.begin(), reversed.end());

    TREES[CTX.chain_trip].target_tree.local_chain.insert(TREES[CTX.chain_trip].target_tree.local_chain.end(), reversed.begin(), reversed.end());
}

// error handler
std::string error(int error_code) {
    json ret = {
        {"FLAG", "ERROR"},
        {"CONTENT", error_code}
    };
    return ret.dump();
}

// - handle functions -

std::string send_blocks(Conn *conn, json args) {
    try {
        #define CTX (conn->message_context)
        #define TREES (*(conn->parent_chains))

        json ret = {
            {"FLAG", "BLOCKS"}
        };
        std::vector<json> blocks;
        for (size_t backi = 0; backi < CTX.k; backi++) {
            size_t workingi = CTX.lastbi - 1 - backi;
            blocks.push_back(json({
                {"#", workingi},
                {"b", TREES[CTX.chain_trip].get_chain()[workingi]}
            }));
            if (workingi == 0) {
                break;
            }
        }
        if (CTX.lastbi > CTX.k) CTX.lastbi = CTX.lastbi - CTX.k;
        else CTX.lastbi = 0;
        ret["CONTENT"] = {{"blocks", blocks}};
        return ret.dump();
    } catch (int err) {
        return error(err);
    }
}

std::string begin_sending_blocks(Conn *conn, json args) {
    try {
        #define CTX (conn->message_context)
        #define TREES (*(conn->parent_chains))

        CTX.chain_trip = args["chain"];
        CTX.lastbi = TREES[CTX.chain_trip].get_chain().size();
        CTX.k = args["k"];
        CTX.pow_min = json(TREES[CTX.chain_trip].get_chain()[0][5])["p"];
        return send_blocks(conn, args);
    } catch (int err) {
        return error(err);
    }
}

std::string evaluate_blocks(Conn *conn, json args) {
    try {
        #define CTX (conn->message_context)
        #define TREES (*(conn->parent_chains))

        json ret = {
        {"FLAG", "ABSENT/V"}
        };
        for (auto block : args["CONTENT"]["blocks"]) {
            size_t bi = block["#"];
            ret["CONTENT"]["i"] = bi;
            if (CTX.wchain.size() > 0 &&
                !(TREES[CTX.chain_trip].verify_block(block, CTX.pow_min)
                    && block["b"][2] == CTX.wchain.back()[1]
                )
            ) {
                ret["FLAG"] == "ABSENT/NV";
                break;
            }
            if (bi < TREES[CTX.chain_trip].get_chain().size()) {
                bool all_equal = true;
                for (size_t i = 0; i < 6; i++) all_equal = (all_equal && TREES[CTX.chain_trip].get_chain()[bi][i] == block["b"][i]);
                if (all_equal) {
                    if (bi == TREES[CTX.chain_trip].get_chain().size() - 1) ret["FLAG"] = "PRESENT/T";
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
// - end of handle functions -

// map of communication roadmap
std::map<std::string /*prev flag*/, std::function<std::string(json args)>> next {
    {"READY", begin_sending_blocks},
    {"BLOCKS", evaluate_blocks},
    {"ABSENT/V", send_blocks}
};

std::string message_logic(Conn *conn) {
    // make sure to;
    // add the socket's info to khosts
    json parsed = json::parse(conn->incoming_msg);
    std::cout << "NEW MSG: " << parsed << "\n";

    // message parsing
    std::string cmd = parsed["FLAG"];
    json args = parsed["CONTENT"];

    // temp return var
    std::string rmsg;

    // client and server roles can both be stored in func map; communication flags ensure proper order of execution
    try {
        rmsg = next[cmd](conn, args);
    } catch (int err) {
        rmsg = error(err);
    }

    // change 'done' to true to end the communication (make sure to return a <close> message)
    // conn_obj->done = true;
    // clean incoming_message for clean recursion
    conn->incoming_msg.clear();
    // return response
    return rmsg;
}