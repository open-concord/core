#include "../source/cpp/miner.cpp"
#include "../source/cpp/block.cpp"
#include <string>
#include <iostream>

int main() {
    std::string test_str = generate_block("testing lmfao", "123");
    Miner testing_miner(1);
    std::cout << "Mining completed!\nNonce was: " << testing_miner.generate_valid_hash(test_str) << "\n";
    return 0;
}