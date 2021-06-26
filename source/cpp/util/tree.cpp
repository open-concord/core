/**
 * This file handles all tree-related logic for the client
 * 
 * NOTES:
 * Timestamping protocol follows the W3 standard;
 * "Complete date plus hours and minutes:
 *  YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)"
 */

#include <string>

// remove after debug
#include <iostream>

#include "../utils.h"
#include "../tree.h"

/*Tree::Tree(int pow_min) {
    this->pow = pow_min;
    (this->local_miner).set_pow_req(this->pow);
}*/

// where h1 is the new content, and h0 is prev hash
std::string Tree::generate_branch(bool debug_info, Miner& local_miner, std::string c1, std::string h0 = "") {
    // don't use h0 if it's not given
    // previously this used a duplicate of c1, but that adds nothing.

    // concatenation of h0+h1/h0
    std::string cat = h0+c1;
    unsigned char nonce[16];

    // debugging
    std::cout << "Catted String: " << cat << "\n";

    // hashing w/ timestamp of h1
    std::string time = get_time();
    local_miner.generate_valid_nonce(debug_info, cat + time, nonce);

    std::string hash = calc_hash(false, cat + time + (char *) nonce);

    // time#prevhash#hash#content
    // all but content have limited chars
    std::string fullBlock = time+h0+hash+(char *) nonce+c1;

    if (debug_info) std::cout << fullBlock.length() << std::endl; //Should be a total of 4(#) + 2*64=128 (hashes) + 22 (timestamp) + 16 (nonce) + messagelength

    return fullBlock;
}

// where h0 and h1 are child nodes, and h01 is block to be checked
bool Tree::verify_integrity(std::string h0, std::string h1, std::string h01) {
    return false;
}