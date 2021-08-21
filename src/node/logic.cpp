#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <functional>

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "../../inc/node.h"
#include "../../inc/strenc.h"
#include "../../inc/crypt++.h"
#include "../../inc/tree.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace boost::placeholders;

/** this file only holds the message logic
 * which will be sure to grow exponentially
 */

// temp var for cfg path
//RW rw_handler;
//json cfg = json::parse(rw_handler.read("../../cfg/main.json"));

void update_chain(Conn *conn) {
    auto CTX = (conn->message_context);
    auto TREE = (*(conn->parent_chains))[CTX.chain_trip];
    std::vector<json> forward_wchain;
    for (size_t i = 0; i < CTX.wchain.size(); i++)
        TREE.chain_push(
            CTX.wchain[CTX.wchain.size() - i]
        );
    conn->parent_blocks_callback(CTX.chain_trip, CTX.wchain.size());
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
// - end of C2C handle functions

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
        //still don't want to connect over loopback. TODO: add a nicer fix
        if (!conn->local) {
            rmsg = ((*next[cmd])(conn, cont)).dump();
        } //else {rmsg = handle_request(conn, cont).dump();}
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