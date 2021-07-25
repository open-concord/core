//#include "../inc/node.h"
//#include "../inc/rw.h"
#include "../inc/crypt++.h"
#include "../inc/tree.h"
#include "../inc/chain_utils.h"
#include "../inc/hash.h"
#include "../inc/b64.h"
#include "../inc/timewizard.h"

#include <string>
#include <vector>
#include <iostream>
#include <exception>

#include <nlohmann/json.hpp>
#include <boost/bind.hpp>

using json = nlohmann::json;

/*
int maint() {

    // port numbers etc will be taken over by cfg files
    unsigned short int host = 1336;
    // create new node
    try {
        // normal node function
        Node tnode (20, host, "test/example_chains_dir", std::vector<std::string>({}));
        // contact with ui 
        // (if you get connection refused, you have to actually open a port w/ the UI on the specified port lmfao)
        // tnode.make_local(1338);
    } catch (std::exception& err) {
        std::cout << "err: " << err.what() << "\n";
    }
    return 0;
}
*/

std::string user_trip = gen_trip(16, 24);
std::string server_trip = gen_trip(16, 24);
std::string aes_key = b64_encode(AES_keygen());
std::array<std::string, 2> dsa_raw_keys = DSA_keygen();
std::string dsa_pri_key = b64_encode(dsa_raw_keys[0]);
std::string dsa_pub_key = b64_encode(dsa_raw_keys[1]); 
Tree test_tree;

void chain_send_message(json data) {
    std::string message = chain_encrypt(
        data,
        dsa_pri_key,
        "", //no RSA key needed
        aes_key,
        's'
    );
    test_tree.generate_branch(
            false, 
            message,
            server_trip,
            user_trip
    );
}

int main() {
    test_tree.set_pow_req(2);
    std::cout << "POW REQ SET\n";

    chain_send_message({
            {"s", user_trip},
            {"ts", get_time()},
            {"t", "nserv"},
            {"d", {
                {"st", server_trip},
                {"sk", aes_key},
                {"nm", user_trip},
                {"pubk", dsa_pub_key}
            }}
    });

    for (size_t i = 0; i < 10; i++) {
        std::cout << "FINISHED ON BLOCK " << std::to_string(i+2) << "\n";
        chain_send_message({
            {"s", user_trip},
            {"ts", get_time()},
            {"t", "msg"},
            {"d", std::to_string(i) + " is the message number"}
        });
    }

    std::vector<json> member_search = chain_search(
        test_tree.get_chain(),
        's',
        server_trip,
        aes_key,
        boost::bind(type_filter, 'p', _1)
    );
    for (auto block : member_search) {
        std::cout << block << "\n";
    }
}