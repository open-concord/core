#include <ctx.hpp>
#include <claf.hpp>

#include <iostream>
#include <string>
#include <map>

int main() {
    std::cout << "initializing node...\n";
    std::map<std::string, Tree*> local_forest;
    std::string tree_trip = gen::trip("seed");
    local_forest[tree_trip] = new Tree("../output/achain/");
    Tree& local_tree = *(local_forest[tree_trip]);
    
    user local_user;
    local_tree.set_pow_req(3);
    
    std::string aes_key = "Xv/azljSEXepU9ThHnfS6mKxLmiw0b90fMm6EsfXF5s=";
    Server local_server(local_tree, aes_key, local_user);
    
    json test_json;
    test_json["c"] = "\nIT WORKS!\n";
    local_server.send_message(local_user, test_json, 'c');
    
    Node local_node(1338);
    local_node.Graph.Forest = local_forest;
    local_node.Contact("127.0.0.1", 1337);
}

