#include <string>
#include <iostream>
#include "../../inc/node.h"

/** this file only holds the message logic
 * which will be sure to grow exponentially
 */

std::string message_logic(Conn *conn_obj) {
    // make sure to add the socket's info to this->khosts

    std::cout << "NEW MSG: " << conn_obj->incoming_msg << "\n";

    // change 'done' to true to end the recursion loop (eg. communication)
    conn_obj->done = true;
    // clean incoming_message for clean recursion
    conn_obj->incoming_msg.clear();
    
    // return response
    return "brochacho";
}