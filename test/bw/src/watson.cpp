#include <tree.hpp>
#include <strops.hpp>
#include <crypt.hpp>
#include <ctx.hpp>

#include <claf.hpp>
#include <hclc.hpp>

#include <iostream>
#include <string>
#include <map>

int main() {
    std::cout << " == initialising graph ==\n";
    std::map<std::string, Tree> local_forest;
    std::string tree_trip = gen::trip("seed");
    local_forest[tree_trip] = Tree("./output/achain");
    Tree& local_tree = local_forest[tree_trip];
    user local_user;
    local_tree.set_pow_req(3);
    std::string aes_key = "Xv/azljSEXepU9ThHnfS6mKxLmiw0b90fMm6EsfXF5s=";
    Server local_server(local_tree, aes_key, local_user);
    std::ifstream art_ifs("ascii.txt");
    std::stringstream art_buffer;
    art_buffer << art_ifs.rdbuf();
    json test_json;
    test_json["c"] = art_buffer.str(); 
    local_server.send_message(local_user, test_json, 'c');
    std::cout << "== initialising node ==\n";  
    Node local_node(1338);
    local_node.Graph.Forest = &local_forest;
    
    std::cout << "== contacting ==\n";
    local_node.Contact("127.0.0.1", 1337);
    auto Conn = local_node.Connections.front().get(); 
    
    std::cout << "== writing ==\n"; 
    hclc _hclc(tree_trip); 
    _hclc.ConnHandle(Conn);
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

