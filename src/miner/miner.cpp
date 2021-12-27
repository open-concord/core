#include <string>
#include <array>
#include <cstring>
#include <sstream>
#include <iomanip>

//debugging
#include <iostream>

#include "../../inc/miner.hpp"
#include "../../inc/strops.hpp"

Miner::Miner(int POW_req) {
    this->pow = POW_req;
};

// checking given hash for compliance w/ chain pow
bool Miner::check_valid_hash(std::string hash) {
    // check first <pow> chars for 0
    int pow_min = this->pow;
    for (int i=0; i<pow_min; i++) {
        if (hash.at(i) != '0') {return false;}
    };
    return true;
};

// genning hash and nonce
std::array<std::string, 2> Miner::generate_valid_nonce(bool debug_info, std::string content) {
    std::string rhash = hex_encode(calc_hash(false, content)); //hash init
    std::string nonce;

    while (!this->check_valid_hash(rhash)) {
        // TODO: add error handling
        nonce = b64_encode(gen_string(16), 24);
        rhash = hex_encode(calc_hash(false, content + nonce));
        if (debug_info) std::cout << "Used nonce " << nonce << " to generate hash " << rhash << std::endl;
    }

    if (debug_info) std::cout << "Succeeded on " << nonce << std::endl;

    return {nonce, rhash};
};
