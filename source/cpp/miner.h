#include <string>

#pragma once

class Miner {
    private:
        int pow;
        std::string hash;
    public:
        Miner(int POW_req);

        // checking given hash for compliance w/ chain pow
        bool check_valid_hash(std::string hash);

        // genning random nonce
        std::string generate_nonce();

        // genning nonce satisfying pow
        std::string generate_valid_nonce(bool debug_info, std::string content);
};