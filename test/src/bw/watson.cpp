#include <tree.hpp>
#include <strops.hpp>
#include <crypt.hpp>
#include <node.hpp>

#include <claf.hpp>
#include <hclc.hpp>

#include <iostream>
#include <string>
#include <map>

int main() {
    std::cout << "initializing node...\n";
    std::map<std::string, Tree> local_forest;
    std::string tree_trip = gen::trip("seed");
    local_forest[tree_trip] = Tree("./output/achain");
    Tree& local_tree = local_forest[tree_trip];
    user local_user;
    local_tree.set_pow_req(3);
    std::string aes_key = "Xv/azljSEXepU9ThHnfS6mKxLmiw0b90fMm6EsfXF5s=";
    Server local_server(local_tree, aes_key, local_user);
    std::ifstream art_ifs("misc/ascii-art.txt");
    std::stringstream art_buffer;
    art_buffer << art_ifs.rdbuf();
    json test_json;
    test_json["c"] = art_buffer.str(); 
    local_server.send_message(local_user, test_json, 'c');
    std::cout << "bp\n";
    Node local_node(
        1338,
        local_forest, 
        [](std::string) {return true;}
      );
    std::cout << "bp1\n";
    local_node.Contact("68.84.7.154", 1337);
    auto Conn = local_node.Connections.front(); 
    /** sanity */
    std::cout << Conn.GraphCtx << '\n';
    Conn.Networking.Raw_Write("sugma");
    std::cout << "bp1.5\n";
    hclc _hclc(tree_trip, 69);
    std::cout << "bp2\n";
    local_node.Connections.front().Networking;
    std::cout << "bp2.5\n";
    //_hclc.ConnHandle(&local_node.Connections.front());
    std::cout << "bp3\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));
}
