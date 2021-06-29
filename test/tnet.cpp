#include "../source/cpp/net/net.h"
#include <iostream>
void handler(std::string msg) {
    std::cout << "MSG: " << msg << "\n";
}

int main () {
    Server test(1339, 20, false);
    test.open_listening();
    // initating new client off the Server's incoming connection stack
    Client next_client(&test);
    next_client.handle(handler);
    return 0;
}