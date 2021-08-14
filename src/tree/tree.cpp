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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>

struct stat info;

Tree::Tree() {
    //nothing here, just needs to be defined for non-dir-linked trees
}

Tree::Tree(std::string dir) {
    load(dir);
}

void Tree::load(std::string dir) {
    this->dir_linked = true;
    this->target_dir = dir;
    if ((this->target_dir).back() != '/') this->target_dir += "/";

    if ((stat( (this->target_dir).c_str(), &info ) != 0) || !(info.st_mode & S_IFDIR) ) {
        throw std::invalid_argument("Directory is not accessible.");
    }

    for (size_t i = 0; true; i++) {
        std::ifstream saved_block((this->target_dir) + std::to_string(i) + ".block");
        if (saved_block) {
            std::string block_data;
            saved_block.seekg(0, std::ios::end); //move to stream end
            block_data.resize(saved_block.tellg()); //expand string based on stream end position
            saved_block.seekg(0, std::ios::beg);
            saved_block.read(&block_data[0], block_data.size());
            chain_push({
                block_data.substr(0, 22), //22 chars of datetime
                block_data.substr(22, 64), //64 chars of last hash
                block_data.substr(22 + 64, 64), //64 chars of current hash
                block_data.substr(22 + 64 + 64, 24), //24 chars of nonce
                block_data.substr(22 + 64 + 64 + 24, 24), //24 chars of nonce creator trip
                block_data.substr(22 + 64 + 64 + 24 + 24, 24), //24 chars of server trip
                block_data.substr(22 + 64 + 64 + 24 + 24 + 24) //remainder is plain content
            });
            saved_block.close();
        }
        else break;
    }
}

void Tree::set_pow_req(int POW_req) {
    this->pow = POW_req;
}

// where h1 is the new content, and h0 is prev hash
void Tree::generate_branch(bool debug_info, std::string c1, std::string st, std::string creator_trip) {
    Miner local_miner(this->pow);
    // concatenation of h0+h1/h0
    assert(st.length() == 24);
    assert(creator_trip.length() == 24);

    std::string h0 = (get_chain().size() > 0) ? get_chain().back()[2] : std::string(64, '0');
    if (debug_info) std::cout << "Old hash: " << h0 << std::endl;

    std::string cat = h0+st+c1;
    std::string nonce;

    // debugging
    if (debug_info) std::cout << "Catted String: " << cat << "\n";

    // hashing w/ timestamp of h1
    std::string time = get_time();

    nonce = local_miner.generate_valid_nonce(debug_info, cat + time);

    std::string hash = calc_hash(false, cat + time + nonce);

    std::vector<std::string> out_block({time, h0, hash, nonce, creator_trip, st, c1});

    (this->local_chain).push_back(out_block);

    save_latest();
}

std::vector<std::vector<std::string>> Tree::get_chain() {
    return (this->local_chain);
}

bool Tree::verify_chain() {
    for (size_t i = 0; i < get_chain().size(); i++) {
        if (!verify_block(get_chain()[i], this->pow)) return false; //make sure every hash is valid.
        if (i != 0) {
            if (get_chain()[i][1] != get_chain()[i - 1][2]) return false; //for blocks beyond the first, ensure hashes chain correctly.
        }
    }
    return true;
}

void Tree::chain_push(std::vector<std::string> block) {
    (this->local_chain).push_back(block);
    save_latest();
}

void Tree::save_latest() {
    if (!dir_linked) return;
    size_t terminal_index = get_chain().size() - 1;
    std::vector<std::string> new_block = get_chain()[terminal_index];
    std::string block_string;
    for (size_t i = 0; i < new_block.size(); i++) block_string+=new_block[i];
    std::ofstream block_file(((this->target_dir) + std::to_string(terminal_index) + ".block").c_str());
    block_file << block_string;
    block_file.close();
}

bool verify_block(std::vector<std::string> block, int pow) {
    std::string result_hash = calc_hash(false, block[1] + block[4] + block[5] + block[0] + block[3]);
    if (result_hash != block[2]) return false;
    for (size_t i = 0; i < pow; i++) {
        if (result_hash.at(i) != '0') return false;
    }
    return true;
}