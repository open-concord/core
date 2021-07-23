//#include "../inc/node.h"
//#include "../inc/rw.h"
#include "../inc/hash.h"
#include "../inc/b64.h"
#include <string>
#include <vector>
#include <iostream>
#include <exception>

/*
int maint() {

    // port numbers etc will be taken over by cfg files
    unsigned short int host = 1336;
    // create new node
    try {
        // normal node function
        Node tnode (20, host, "test/example_chains_dir", std::vector<std::string>({}));
        // contact with ui 
        // (if you get connection refused, you have to actually open a port w/ the UI on the specified port lmfao)
        // tnode.make_local(1338);
    } catch (std::exception& err) {
        std::cout << "err: " << err.what() << "\n";
    }
    return 0;
}
*/

int main() {
    std::cout << b64_decode(b64_encode("hdhfjkghfhdrstfjykuujyhtgefd", 100)) << std::endl;
    std::cout << calc_hash(false, "rawraw") << std::endl;
    std::cout << calc_hash(false, "rawrar") << std::endl;
    std::cout << calc_hash(false, "rawraw").length() << std::endl;
    std::cout << calc_hash(false, "rawrar").length() << std::endl;
}