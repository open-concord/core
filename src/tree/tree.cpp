#include <string>
#include <array>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <experimental/algorithm>
#include <random>
#include <iostream>

#include "../../inc/miner.h"
#include "../../inc/hash.h"
#include "../../inc/tree.h"
#include "../../inc/strenc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>
#include <boost/function.hpp>
#include <boost/bind/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

namespace fs = boost::filesystem;

using namespace boost::placeholders;

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

    std::vector<block> loaded_blocks;

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
            loaded_blocks.push_back(parsed_block);
        }
        else continue;
    }
    batch_push(loaded_blocks, false);
}

void Tree::set_pow_req(int POW_req) {
    this->pow = POW_req;
}

std::string Tree::gen_block(std::string cont, std::string s_trip, unsigned long long set_time, std::unordered_set<std::string> p_hashes, std::string c_trip) {
    assert(s_trip.length() == 24);
    assert(c_trip.length() == 24);
    if (p_hashes.size() == 0) p_hashes = find_p_hashes(s_trip);
    block out_block = construct_block(cont, p_hashes, this->pow, s_trip, set_time, c_trip);
    chain_push(out_block);
    return out_block.hash;
}

std::unordered_set<std::string> Tree::find_p_hashes(std::string s_trip, std::unordered_set<std::string> base_p_hashes, int p_count) {
    //there's no point in using blocks with existing children as hashes; we can get the same reliance by using their children
    std::unordered_set<std::string> p_hashes = base_p_hashes;

    std::unordered_set<std::string> intra_childless_hashes = get_qualifying_hashes(boost::bind(&Tree::is_intraserver_childless, _1, _2, s_trip)); 

    std::sample(
        intra_childless_hashes.begin(),
        intra_childless_hashes.end(),
        std::inserter(p_hashes, p_hashes.begin()),
        std::min((int) (p_count - p_hashes.size()), 1),
        std::mt19937{std::random_device{}()}
    );
    int p_remainder = p_hashes.size() - p_count;
    if (p_remainder > 0) {
        std::unordered_set<std::string> childless_hashes = get_qualifying_hashes(&Tree::is_childless);
        std::sample(
            childless_hashes.begin(),
            childless_hashes.end(),
            std::inserter(p_hashes, p_hashes.begin()),
            p_remainder,
            std::mt19937{std::random_device{}()}
        );
    }
    return p_hashes;
}

std::map<std::string, block> Tree::get_chain() {
    return (this->chain);
}

bool Tree::verify_chain() {
    std::map<std::string, bool> s_trip_seen;
    for (const auto& [hash, block] : get_chain()) {
        if (!verify_block(block, this->pow)) return false; //make sure every hash is valid.
        //If there were multiple server roots, we couldn't tell which was valid.
        //P2P prevents the propagation of new roots, but this means we have
        //to fail if there's more than one root
        bool server_connected = false;
        for (auto ph : block.p_hashes) {
            if (get_chain().find(ph) == get_chain().end()) return false; //parent hashes all need to exist in the chain
            if (get_chain()[ph].s_trip == block.s_trip) server_connected = true;
        }
        if (!server_connected) {
            s_trip_seen[block.s_trip] = !s_trip_seen[block.s_trip]; //first will make true, second back to false
            if (!s_trip_seen[block.s_trip]) return false; 
            //if it's false (was true before, i.e. was flipped, so this is the second sighting) we fail
        }
    }
    return true;
}

bool Tree::is_childless(block to_check) {
    return (to_check.c_hashes.size() == 0);
}

bool Tree::is_orphan(block to_check) {
    return (to_check.p_hashes.size() == 0);
}

bool Tree::is_intraserver_childless(block to_check, std::string server_trip) {
    if (to_check.s_trip != server_trip) return false;
    for (auto ch : to_check.c_hashes) {
        if (get_chain()[ch].s_trip == server_trip) return false;
    }
    return true;
}

bool Tree::is_intraserver_orphan(block to_check, std::string server_trip) {
    if (to_check.s_trip != server_trip) return false;
    for (auto ch : to_check.p_hashes) {
        if (get_chain()[ch].s_trip == server_trip) return false;
    }
    return true;
}

int Tree::intraserver_child_count(block to_check, std::string server_trip) {
    if (to_check.s_trip != server_trip) return 0;
    int result = 0;
    for (auto ch : to_check.c_hashes) {
        if (get_chain()[ch].s_trip == server_trip) result++;
    }
    return result;
}

int Tree::intraserver_parent_count(block to_check, std::string server_trip) {
    if (to_check.s_trip != server_trip) return 0;
    int result = 0;
    for (auto ch : to_check.p_hashes) {
        if (get_chain()[ch].s_trip == server_trip) result++;
    }
    return result;
}

std::unordered_set<std::string> Tree::get_qualifying_hashes(boost::function<bool(Tree*, block)> qual_func) {
    std::unordered_set<std::string> qualifying_hashes;
    for (const auto& [hash, block] : get_chain()) {
        if (qual_func(this, block)) qualifying_hashes.insert(hash);
    }
    return qualifying_hashes;
}

std::unordered_set<std::string> Tree::get_parent_hash_union(std::unordered_set<std::string> c_hashes) {
    std::unordered_set<std::string> p_hash_union;
    for (const auto& ch: c_hashes) {
        for (const auto& ph : get_chain()[ch].p_hashes) {
            p_hash_union.insert(ph);
        }
    }
    return p_hash_union;
}

void Tree::chain_push(block to_push) {
    (this->chain)[to_push.hash] = to_push;
    link_block(to_push);
    save(to_push);
}

void Tree::batch_push(std::vector<block> to_push_set, bool save_new) {
    for (const auto& to_push_block : to_push_set) {
        (this->chain)[to_push_block.hash] = to_push_block;
    }
    for (const auto& [hash, block] : get_chain()) {
        link_block(block);
    }
    if (save_new) {
        for (const auto& pushed_block : to_push_set) { 
            save(pushed_block);
        }
    }
}

void Tree::link_block(block to_link) {
    for (auto ph : to_link.p_hashes) {
        (this->chain)[ph].c_hashes.insert(to_link.hash);
    }
}

void Tree::save(block to_save) {
    if (!dir_linked) return;
    std::string block_string = block_to_json(to_save).dump();
    std::ofstream block_file(((this->target_dir) + to_save.hash + ".block").c_str());
    block_file << block_string;
    block_file.close();
}

std::vector<std::string> order_hashes(std::unordered_set<std::string> input_hashes) {
    std::vector<std::string> hashes_vector;
    std::copy(input_hashes.begin(), input_hashes.end(), std::back_inserter(hashes_vector));
    std::sort(hashes_vector.begin(), hashes_vector.end(), std::greater<std::string>());
    return hashes_vector;
}

std::string hash_concat(block input) {
    std::string concat_data = b64_encode(raw_time_to_string(input.time)) + input.s_trip + input.cont; //b64 timestr encoding is only for safety
    for (auto ph : order_hashes(input.p_hashes)) concat_data += ph;
    return concat_data;
}

bool verify_block(block to_verify, int pow) {

    std::string result_hash = hex_encode(calc_hash(false, hash_concat(to_verify) + to_verify.nonce));
    if (result_hash != to_verify.hash) return false;
    for (int i = 0; i < pow; i++) {
        if (result_hash.at(i) != '0') return false;
    }
    return true;
}

block construct_block(std::string cont, std::unordered_set<std::string> p_hashes, int pow, std::string s_trip, unsigned long long set_time, std::string c_trip) {
    Miner local_miner(pow);

    block output;
    output.time = set_time;
    output.s_trip = s_trip;
    output.c_trip = c_trip;
    output.cont = cont;
    output.p_hashes = p_hashes;
    std::array<std::string, 2> nonce_result = local_miner.generate_valid_nonce(false, hash_concat(output));
    output.nonce = nonce_result[0];
    output.hash = nonce_result[1];
    return output;
}