#include "concord/tree.h"
#include "concord/hash.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "OF" << std::endl;
    std::string server_trip = gen_trip(16, 24);

    Tree example("example_chain_dir");

    example.set_pow_req(3);

    example.gen_block("F1rst message", server_trip);
    for (int i = 2; i < 5; i++) example.gen_block(std::to_string(i) + "th message", server_trip);

    for (const auto& [hash, block] : example.get_chain()) {
        std::cout << block_to_json(block).dump() << "\n";
    }

    std::cout << example.verify_chain() << "\n";
    return 0;
}