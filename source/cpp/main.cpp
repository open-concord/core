#include "ledger.cpp"
#include "utils.h"
#include "tree.h"

int main() {
    /*
    Ledger testing;
    testing.start();
    testing.shutdown();
    */
    Miner tminer(3);
    Tree ttree;
    unsigned char result_nonce[16];
    
    std::cout << ttree.generate_branch(true, tminer, "hello", "") << std::endl;

    return 0;
}