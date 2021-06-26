#include <string>
#include <openssl/rand.h>
#include <cstring>

//debugging
#include <iostream>

#include "../utils.h"

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

std::string Miner::generate_nonce() {
    unsigned char nonce[16];

    RAND_bytes(nonce, sizeof(nonce));

    return std::string((char *) nonce);
}

// genning hash and nonce
std::string Miner::generate_valid_nonce(bool debug_info, std::string content) {
    std::string rhash = calc_hash(false, content); //hash init
    std::string nonce;

    while (!this->check_valid_hash(rhash)) {
        // TODO: add error handling
        nonce = generate_nonce();
        rhash = calc_hash(false, content + nonce);
        if (debug_info) std::cout << "Used nonce " << nonce << " to generate hash " << rhash << std::endl;
    }

    if (debug_info) std::cout << "Succeeded on " << nonce << std::endl;

    return nonce;
};