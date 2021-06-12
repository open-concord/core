// blockchain management + block propagation
// this is where the isolation of each ledger comes into effect
#include <string>
#include <chrono>
#include <iostream>

std::string generate_block(std::string content, std::string prevhash) {
    // get epoch (timestamp)
    const auto t = std::chrono::system_clock::now();
    int epoch = std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch()).count();
    
    std::string sepoch = std::to_string(epoch);

    // text formatting
    std::string sep = "#";
    //data#timestamp#prevhash
    std::string ret = content+=sep+=sepoch+=sep+=prevhash;
    std::cout << "Original string: " << ret << "\n";
    return ret;
}