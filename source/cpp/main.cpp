#include "ledger.cpp"
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
    Miner tminer(3);
    FileTree tftree("../../test/example_ledger/");
    
    for (auto dataPiece : dataPieces) {
        tftree.generate_branch(false, tminer, dataPiece);
    }



    for (auto block : tftree.get_chain()) {
        std::cout << "BEGIN CONT: " << block[4] << " :END CONT"<< std::endl;
        std::cout << "BEGIN OLDHASH: " << block[1] << " :END OLDHASH"<< std::endl;
        std::cout << "BEGIN NEWHASH: " << block[2] << " :END NEWHASH"<< std::endl;
    }

    return 0;
}