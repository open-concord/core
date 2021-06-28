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

// remove after debug
#include <iostream>

#include "../utils.h"
#include "../tree.h"

/*Tree::Tree(int pow_min) {
    this->pow = pow_min;
    (this->local_miner).set_pow_req(this->pow);
}*/

// where h1 is the new content, and h0 is prev hash
void Tree::generate_branch(bool debug_info, Miner& local_miner, std::string c1) {
    // concatenation of h0+h1/h0
    std::string h0 = ((this->local_chain).size() > 0) ? (this->local_chain).back()[2] : std::string(64, '0');
    if (debug_info) std::cout << "Old hash: " << h0 << std::endl;

    std::string cat = h0+c1;
    std::string nonce;

    // debugging
    if (debug_info) std::cout << "Catted String: " << cat << "\n";

    // hashing w/ timestamp of h1
    std::string time = get_time();

    nonce = local_miner.generate_valid_nonce(debug_info, cat + time);

    std::string hash = calc_hash(false, cat + time + nonce);

    std::cout << "NSIZE: " << nonce.length() << std::endl;

    std::array<std::string, 5> out_block = {time, h0, hash, nonce, c1};

    (this->local_chain).push_back(out_block);
}

std::vector<std::array<std::string, 5>> Tree::get_chain() {
    return (this->local_chain);
}

// where h0 and h1 are child nodes, and h01 is block to be checked
bool Tree::verify_integrity(std::string h0, std::string h1, std::string h01) {
    return false;
}