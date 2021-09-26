#include "concord/tree.h"
#include "concord/hash.h"
#include "concord/strenc.h"
#include "concord/crypt++.h"
#include "concord/server.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "OF" << std::endl;
    std::string raw_aes = AES_keygen();

    Tree example; //"example_chain_dir");

    user example_user;

    example.set_pow_req(3);

    Server server1(example, b64_encode(raw_aes), example_user);

    std::cout << "Made server\n";

    std::cout << server1.get_root_branch().messages[0].data << "\n";

    for (const auto& [hash, block] : example.get_chain()) {
        std::cout << block_to_json(block).dump() << "\n";
    }

    std::cout << example.verify_chain() << "\n";
    return 0;
}