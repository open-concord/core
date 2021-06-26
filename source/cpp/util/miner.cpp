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

// genning hash and nonce
int Miner::generate_valid_nonce(bool debug_info, std::string content, unsigned char* nonce) {
    std::string rhash = calc_hash(false, content); //hash init
    //unsigned char* hashInput;
    unsigned char working_nonce[16]; // nonce buffer

    //hashInput = malloc(16);

    while (!this->check_valid_hash(rhash)) {
        // TODO: add error handling
        RAND_bytes(working_nonce, sizeof(working_nonce));
        rhash = calc_hash(false, content + (char*) working_nonce);
        if (debug_info) std::cout << "Used nonce " << working_nonce << " to generate hash " << rhash << std::endl;
    }

    if (debug_info) std::cout << "Succeeded on " << working_nonce << std::endl;
    memcpy(nonce, working_nonce, sizeof(working_nonce) + 1); //16 + the termination char.

    return 1;
};