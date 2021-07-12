#include "../source/cpp/util/tree.cpp"
#include <string>
#include <iostream>

// intiate new create new tree branch
int main() {

    Tree testTree;
    // prev hash is just QWERTYUIOPQWERTYUIOP (aka genesis node)
    std::string branchHash = testTree.generate_branch("ASDFGHJKL", "548e842c65b83cd83ca76ea6b32d59d84b2d444fc6ff4409e522cc5689934eea");
    
    std::cout << branchHash << "\n";

    return 0;
}