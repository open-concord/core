#include "../inc/node.h"
#include "../inc/rw.h"
#include "../inc/crypt++.h"
#include "../inc/tree.h"
#include "../inc/chain_utils.h"
#include "../inc/hash.h"
#include "../inc/b64.h"
#include "../inc/timewizard.h"


#include <string>
#include <vector>
#include <iostream>
#include <exception>

#include <nlohmann/json.hpp>

int main() {
    // port numbers etc will be taken over by cfg files
    unsigned short int host = 1338;
    // create new node
    try {
        // normal node function
        Node tnode (20, host, "test/example_chains_dir", std::vector<std::string>({}));
        // contact with ui 
        // (if you get connection refused, you have to actually open a port w/ the UI on the specified port lmfao)
        tnode.make_local(host);
    } catch (std::exception& err) {
        std::cout << "err: " << err.what() << "\n";
    }
    return 0;
}