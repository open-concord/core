#include "concord/tree.h"
#include "concord/strops.h"
#include "concord/crypt++.h"
#include "concord/server.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "OF" << std::endl;
    std::string raw_aes = AES_keygen();

    Tree example("example_chain_dir/");

    std::cout << example.get_chain().size() << "\n";
    std::cout << "MARK A\n";

    user example_user;

    example.set_pow_req(3);

    std::cout << "MARK B\n";

    Server server1(example, b64_encode(raw_aes), example_user);

    std::cout << "Made server\n";

    json test;

    test["data"] = "test_message";

    std::cout << test << "\n";

    /*
    std::string msg = server1.send_message(example_user, test, 'c');
    
    std::cout << msg << "\n";
    //*/

    branch rb = server1.get_root_branch();
    
    std::cout << "Fetched root branch\n";

    std::cout << rb.messages.size() << "\n";

    std::cout << example.verify_chain() << "\n";
    return 0;
}