#include "../inc/tree.h"
#include "../inc/hash.h"
#include "../inc/node.h"
#include <string>
#include <vector>
#include <iostream>

int main() {

    // initiate contact with GUI
    try {

    } catch (errno) {
        std::cout << "ERRNO: " << errno << "\n";
    }


    /*
    Ledger testing;
    testing.start();
    testing.shutdown();
    */
    std::vector<std::string> dataPieces;
    Miner tminer(4);
    FileTree tftree("test/example_ledger/");

    for (size_t i = 0; i < 6000; i++) {
        dataPieces.push_back("This is the " + std::to_string(i) + "th block.");
    }
    
    for (auto dataPiece : dataPieces) {
        tftree.generate_branch(false, tminer, dataPiece, gen_trip());
        //std::cout << "Completed '" << dataPiece << "'" << std::endl;
    }

    for (auto block : tftree.get_chain()) {
        //std::cout << "BEGIN SUCCESS: " << tftree.verify_integrity(block) << " :END SUCCESS" << std::endl;
        std::cout << "BEGIN CONT: " << block[5] << " :END CONT"<< std::endl;
        std::cout << "BEGIN OLDHASH: " << block[1] << " :END OLDHASH"<< std::endl;
        std::cout << "BEGIN NEWHASH: " << block[2] << " :END NEWHASH"<< std::endl;
    }

    std::cout << tftree.verify_chain(4) << std::endl;

    return 0;
}