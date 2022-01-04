#include <tree.hpp>
#include <strops.hpp>
#include <crypt++.hpp>
#include <server.hpp>
#include <node.hpp>
#include <iostream>
#include <string>
#include <map>

int main() {
    std::cout << "initializing node...\n";
    std::map<std::string, Tree> local_forest;
    std::string tree_trip = gen_trip("seed");
    local_forest[tree_trip] = Tree("example_chain_dir/");
    Tree& local_tree = local_forest[tree_trip];
    user local_user;
    local_tree.set_pow_req(3);
    /*std::string aes_key = "Xv/azljSEXepU9ThHnfS6mKxLmiw0b90fMm6EsfXF5s=";
    Server local_server(local_tree, aes_key, local_user);
    std::ifstream art_ifs("misc/ascii-art.txt");
    std::stringstream art_buffer;
    art_buffer << art_ifs.rdbuf();
    json test_json;
    test_json["c"] = "\nIT WORKS!\n";
    local_server.send_message(local_user, test_json, 'c');*/
    Node local_node(5, 1338, local_forest, 15000, [](std::string ip) {return true;});
    local_node.Contact(tree_trip, 1, "127.0.0.1", 1337);
}
