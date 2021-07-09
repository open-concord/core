#include "../../inc/node.h"

int main() {
    bool finished = false;
    while (!finished) {
        // initiate node on socket
        Node net(20, 1338);
        net.start();
        net.begin_next();
    }
    return 0;
}