/**
 * This file handles all tree-related logic for the client
 * 
 * NOTES:
 * Timestamping protocol follows the W3 standard;
 * "Complete date plus hours and minutes:
 *  YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)"
 */

#include <string>
#include <vector>
#include <array>
#include <cassert>
#include <iostream>

#include "../../inc/miner.h"
#include "../../inc/hash.h"
#include "../../inc/timewizard.h"
#include "../../inc/tree.h"

// where h1 is the new content, and h0 is prev hash
void Tree::generate_branch(bool debug_info, Miner& local_miner, std::string c1, std::string st) {
    // concatenation of h0+h1/h0
    assert(st.length() == 32);

    std::string h0 = ((this->local_chain).size() > 0) ? (this->local_chain).back()[2] : std::string(64, '0');
    if (debug_info) std::cout << "Old hash: " << h0 << std::endl;

    std::string cat = h0+st+c1;
    std::string nonce;

    // debugging
    if (debug_info) std::cout << "Catted String: " << cat << "\n";

    // hashing w/ timestamp of h1
    std::string time = get_time();

    nonce = local_miner.generate_valid_nonce(debug_info, cat + time);

    std::string hash = calc_hash(false, cat + time + nonce);

    std::vector<std::string> out_block({time, h0, hash, nonce, st, c1});

    (this->local_chain).push_back(out_block);
}

std::vector<std::vector<std::string>> Tree::get_chain() {
    return (this->local_chain);
}

bool Tree::verify_block(std::vector<std::string> block, int pow_min) {
    std::string result_hash = calc_hash(false, block[1] + block[4] + block[5] + block[0] + block[3]);
    if (result_hash != block[2]) return false;
    for (size_t i = 0; i < pow_min; i++) {
        if (result_hash.at(i) != '0') return false;
    }
    return true;
}

bool Tree::verify_chain(int pow_min) {
    for (size_t i = 0; i < (this->local_chain).size(); i++) {
        if (!verify_block((this->local_chain)[i], pow_min)) return false; //make sure every hash is valid.
        if (i != 0) {
            if ((this->local_chain)[i][1] != (this->local_chain)[i - 1][2]) return false; //for blocks beyond the first, ensure hashes chain correctly.
        }
    }
    return true;
}