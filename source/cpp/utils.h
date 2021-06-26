#include <string>

#pragma once

std::string get_time();

std::string convert_to_local();

std::string calc_hash (bool use_disk, std::string target);

class Miner {
    private:
        int pow;
        std::string hash;
    public:
        Miner(int POW_req);

        // checking given hash for compliance w/ chain pow
        bool check_valid_hash(std::string hash);

        // genning nonce satisfying pow
        int generate_valid_nonce(bool debug_info, std::string content, unsigned char* nonce);
};