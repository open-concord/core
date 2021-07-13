#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include "../../inc/node.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/** this file only holds the message logic
 * which will be sure to grow exponentially
 */

// error handler
json error(int error_code) {
    json ret = {
        {"FLAG", "ERROR"},
        {"CONTENT", error_code}
    };
    return ret;
}

// - external handle functions -
json begin_sending_blocks(json cont) {
    try {
        // check for content, if flawed throw error

        // return specified block in specified amount
    } catch (int err) {
        return error(err);
    }
}

json evaluate_blocks(json cont) {
    try {
        // check each subsequent block, see contact.txt

        // return status
    } catch (int err) {
        return error(err);
    }
}
// - end of external handle functions -

// - local handle functions -
json online(json cont) {
    try {
        std::cout << "MSG: " << cont.dump() << "\n";
    } catch (int error) {
        return error(err);
    }
}
// - end of local handle functions -

// map of communication roadmap
std::map<std::string /*prev flag*/, std::function<json(json cont)>> next {
    {"READY", begin_sending_blocks},
    {"BLOCKS", evaluate_blocks}
};

// map for local comms
std::map<std::string/*flag*/, std::function<json(json cont)>> lnext {
    {"ONLINE", online}
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
        if (conn->local) {
            rmsg = lnext[cmd](cont).dump();
        } else {rmsg = next[cmd](cont).dump();}
    } catch (int err) {
        rmsg = error(err).dump();
    }

    // change 'done' to true to end the communication (make sure to return a <close> message)
    // conn_obj->done = true;
    // clean incoming_message for clean recursion
    conn->incoming_msg.clear();
    // update message_context
    conn->message_context[cmd] = cont;
    // return response
    return rmsg;
}