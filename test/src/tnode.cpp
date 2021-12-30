#include <node.hpp>
#include <tree.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <map>

bool watchdog(std::string ip) {
  return true;
}

int main() {
    // port numbers etc will be taken over by cfg files
    unsigned short int host = 1338;
    // create new node
    Tree new_tree;
    std::map<std::string, Tree> test_ledger = {
      {"qwerty", new_tree}
    };
    try {
      // normal node function
      Node tnode (
        5,
        host,
        test_ledger,
        3000, /** ms */
        &watchdog
      );
      
    } catch (std::exception& err) {
      std::cout << "err: " << err.what() << "\n";
    }
    return 0;
}
