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
    std::vector<std::string> blocks({""});
    Miner tminer(3);
    Tree ttree;
    unsigned char result_nonce[16];
    
    for (size_t i = 0; i < dataPieces.size(); i++) {
        std::string last_block_hash = (blocks[i].length() != 0) ? blocks[i].substr(22 + 64, 64) : "";
        std::cout << "BEGIN HASH: " << last_block_hash << " :END HASH" << std::endl;
        blocks.push_back(ttree.generate_branch(false, tminer, dataPieces[i], last_block_hash));
    }

    for (auto block : blocks) {
        std::cout << "BEGIN BLOCK: " << block << " :END BLOCK" << std::endl << std::endl;
    }

    return 0;
}