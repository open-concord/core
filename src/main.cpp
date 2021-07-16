#include "../inc/node.h"
#include "../inc/rw.h"
#include <string>
#include <vector>
#include <iostream>
#include <exception>

int main() {

    // port numbers etc will be taken over by cfg files
    unsigned short int host = 1336;
    // create new node
    try {
        // normal node function
        Node tnode (20, host);
        // contact with ui 
        // (if you get connection refused, you have to actually open a port w/ the UI on the specified port lmfao)
        // tnode.make_local(1338);
    } catch (std::exception& err) {
        std::cout << "err: " << err.what() << "\n";
    }
    return 0;
}