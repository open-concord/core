#include <string>
#include <vector>
#include <array>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <experimental/algorithm>
#include  <random>

#include "../../inc/miner.h"
#include "../../inc/hash.h"
#include "../../inc/tree.h"
#include "../../inc/b64.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

namespace fs = boost::filesystem;

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

    fs::path p(this->target_dir);

    for(auto& entry : boost::make_iterator_range(fs::directory_iterator(p), {})) {
        std::string path_str = entry.path().string();
        if (path_str.substr(path_str.length() - 6) != ".block") continue; //only want .block files
        std::ifstream saved_block(path_str.c_str());
        if (saved_block) {
            std::string block_data;
            saved_block.seekg(0, std::ios::end); //move to stream end
            block_data.resize(saved_block.tellg()); //expand string based on stream end position
            saved_block.seekg(0, std::ios::beg);
            saved_block.read(&block_data[0], block_data.size());
            saved_block.close();
            block parsed_block = json_to_block(json::parse(block_data));
            (this->chain)[parsed_block.hash] = parsed_block;
        }
        else continue;
    }
    //now that we've loaded the chain, we need to link it (since we can't be sure of load order, we need to link after everything is available)
    for (const auto& [hash, block] : get_chain()) {
        link_block(block);
    }
}

void Tree::set_pow_req(int POW_req) {
    this->pow = POW_req;
}

void Tree::gen_block(std::string cont, std::string s_trip, int p_count, std::string c_trip) {
    assert(s_trip.length() == 24);
    assert(c_trip.length() == 24);
    std::vector<std::string> childless_hashes;
    std::vector<std::string> p_hashes;
    for (const auto& [hash, block] : get_chain()) {
        if (block.c_hashes.size() == 0) childless_hashes.push_back(hash);
    }
    if (!get_chain().empty()) assert(childless_hashes.size() > 0); //0 childless hashes means a cyclical chain
    //there's no point in using blocks with existing children as hashes; we can get the same reliance by using their children
    std::experimental::sample(
        childless_hashes.begin(),
        childless_hashes.end(),
        std::back_inserter(p_hashes),
        p_count,
        std::mt19937{std::random_device{}()}
    );
    chain_push(construct_block(cont, p_hashes, this->pow, s_trip, c_trip));
}

std::map<std::string, block> Tree::get_chain() {
    return (this->chain);
}

bool Tree::verify_chain() {
    for (const auto& [hash, block] : get_chain()) {
        if (!verify_block(block, this->pow)) return false; //make sure every hash is valid.
        for (auto ph : block.p_hashes) {
            if (get_chain().find(ph) == get_chain().end()) return false; //parent hashes all need to exist in the chain
        }
    }
    return true;
}

void Tree::chain_push(block to_push) {
    (this->chain)[to_push.hash] = to_push;
    link_block(to_push);
    save(to_push);
}

void Tree::link_block(block to_link) {
    for (auto ph : to_link.p_hashes) {
        (this->chain)[ph].c_hashes.push_back(to_link.hash);
    }
}

void Tree::save(block to_save) {
    if (!dir_linked) return;
    std::string block_string = block_to_json(to_save).dump();
    std::ofstream block_file(((this->target_dir) + to_save.hash + ".block").c_str());
    block_file << block_string;
    block_file.close();
}

std::string hash_concat(block input) {
    std::string concat_data = b64_encode(raw_time_to_string(input.time)) + input.s_trip + input.cont; //b64 timestr encoding is only for safety
    for (auto ph : input.p_hashes) concat_data += ph;
    return concat_data;
}

bool verify_block(block to_verify, int pow) {

    std::string result_hash = calc_hash(false, hash_concat(to_verify) + to_verify.nonce);
    if (result_hash != to_verify.hash) return false;
    for (int i = 0; i < pow; i++) {
        if (result_hash.at(i) != '0') return false;
    }
    return true;
}

block construct_block(std::string cont, std::vector<std::string> p_hashes, int pow, std::string s_trip, std::string c_trip) {
    Miner local_miner(pow);

    unsigned long long time = get_raw_time();
    block output;
    output.time = time;
    output.s_trip = s_trip;
    output.c_trip = c_trip;
    output.cont = cont;
    output.p_hashes = p_hashes;
    std::string concat_data = hash_concat(output);
    output.nonce = local_miner.generate_valid_nonce(false, concat_data);
    output.hash = calc_hash(false, concat_data + output.nonce);
    return output;
}