#include "../source/cpp/block.cpp"
#include <string>

// intiate new chain/block
int main() {

    std::string test = generate_block("hi this is a testing message", "000000");
    
    return 0;
}