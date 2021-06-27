#include "ledger.cpp"
#include "utils.h"
#include "tree.h"
#include <string>
#include <vector>
#include <iostream>

int main() {
    /*
    Ledger testing;
    testing.start();
    testing.shutdown();
    */
    std::vector<std::string> dataPieces({"Hector", "Alexander", "Julius", "Joshua", "David", "Judah Maccabee", "Arthur", "Charlegmagne", "Godfrey"});
    Miner tminer(5);
    Tree ttree;
    unsigned char result_nonce[16];
    
    for (auto dataPiece : dataPieces) {
        ttree.generate_branch(false, tminer, dataPiece);
    }



    for (auto block : ttree.localChain) {
        std::cout << "BEGIN CONT: " << block[4] << " :END CONT"<< std::endl;
        std::cout << "BEGIN OLDHASH: " << block[1] << " :END OLDHASH"<< std::endl;
        std::cout << "BEGIN NEWHASH: " << block[2] << " :END NEWHASH"<< std::endl;

    }

    return 0;
}