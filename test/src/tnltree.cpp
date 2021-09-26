#include "concord/tree.h"
#include "concord/hash.h"
#include "concord/strenc.h"
#include "concord/crypt++.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "OF" << std::endl;
    std::string raw_aes1 = AES_keygen();
    std::string raw_aes2 = AES_keygen();
    std::string server_trip1 = gen_trip(b64_encode(raw_aes1), 24);
    std::string server_trip2 = gen_trip(b64_encode(raw_aes2), 24);

    Tree example; //"example_chain_dir");

    example.set_pow_req(3);

    example.gen_block("F1rst message", server_trip1);
    std::string second_server_hash = example.gen_block("Other_server_message", server_trip2);
    //example.gen_block("Alternative first message", server_trip1, std::unordered_set<std::string>({second_server_hash}));
    for (int i = 2; i < 5; i++) example.gen_block(std::to_string(i) + "th message", server_trip1);

    for (const auto& [hash, block] : example.get_chain()) {
        std::cout << block_to_json(block).dump() << "\n";
    }

    std::cout << example.verify_chain() << "\n";
    return 0;
}