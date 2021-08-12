#include "node.h"
#include "rw.h"
#include "crypt++.h"
#include "tree.h"
#include "chain_utils.h"
#include "hash.h"
#include "b64.h"
#include "timewizard.h"


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