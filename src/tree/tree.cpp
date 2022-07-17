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
#include <filesystem>

#include "../../inc/tree.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>

struct stat info;

Tree::Tree() {
  // @Overload
  // nothing here, just needs to be defined for non-dir-linked trees
}

Tree::Tree(std::string dir) {
    load(dir);
}

void Tree::load(std::string dir) {
    this->dir_linked = true;
    this->target_dir = dir;
    if ((this->target_dir).back() != '/') this->target_dir += "/";
    
    struct stat info;
    /** right now, this is hardcoded file dir.
     * TODO: Allow FIFO, socket, etc.
     */ 
    if ((stat( (this->target_dir).c_str(), &info ) != 0) || (info.st_mode & S_IFMT) != S_IFDIR) {
        int err = errno;
        if (err) {std::cout << "[!] errno: " << err << '\n';}
        throw std::invalid_argument("Directory is not accessible.");
    }

    std::filesystem::path p(this->target_dir);

    std::vector<block> loaded_blocks;

    for(auto& entry : std::filesystem::directory_iterator(p)) {
        std::string path_str = entry.path().string();
        if (path_str.substr(path_str.length() - 6) != ".block") continue; // only want .block files
        std::ifstream saved_block(path_str.c_str());
        if (saved_block) {
            std::string block_data;
            saved_block.seekg(0, std::ios::end); // move to stream end
            block_data.resize(saved_block.tellg()); // expand string based on stream end position
            saved_block.seekg(0, std::ios::beg);
            saved_block.read(&block_data[0], block_data.size());
            saved_block.close();
            block parsed_block(json::parse(block_data));
            loaded_blocks.push_back(parsed_block);
        }
        else continue;
    }
    batch_push(loaded_blocks, false);
}

void Tree::set_pow_req(int POW_req) {
    this->pow = POW_req;
}

std::string Tree::gen_block(
  std::string cont,
  std::string s_trip,
  unsigned long long set_time,
  std::unordered_set<std::string> p_hashes,
  std::string c_trip) {
    assert(s_trip.length() == 24);
    assert(c_trip.length() == 24 || c_trip.length() == 0);
    if (p_hashes.size() == 0) p_hashes = find_p_hashes(s_trip);
    block out_block(cont, p_hashes, this->pow, s_trip, set_time, c_trip);
    chain_push(out_block);
    return out_block.hash;
}

std::unordered_set<std::string> Tree::find_p_hashes(std::string s_trip, std::unordered_set<std::string> base_p_hashes, int p_count) {
    /**
    * there's no point in using blocks with existing children as hashes;
    * we can get the same reliance by using their children
    */
    std::unordered_set<std::string> p_hashes = base_p_hashes;

    std::unordered_set<std::string> intra_childless_hashes = get_qualifying_hashes(&Tree::is_intraserver_childless);

    /**
    * if the base hashes have an intraserver block,
    * we don't require another one, so the minimum intra sample is 0.
    * Otherwise, it's 1 for server continuity.
    */
    bool require_intra_block = true;

    for (const auto& bp_hash : p_hashes) {
        if (get_chain()[bp_hash].s_trip == s_trip) {
            require_intra_block = false;
            break;
        }
    }

    std::sample(
        intra_childless_hashes.begin(),
        intra_childless_hashes.end(),
        std::inserter(p_hashes, p_hashes.begin()),
        std::max((int) (p_count - p_hashes.size()), (int) require_intra_block), //we want at least one block from the server for continuity, but if we already have one we just need to fill the p_count.
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

std::vector<block> Tree::search_user(std::string trip) {
  std::vector<block> matches; 
  for (const auto& [_hash, _block] : this->get_chain()) {
    try {
      json j = json::parse(_block.cont);
      if (!j["d"] || !(_block.verify(this->pow))) {throw;}
      if (j["d"] == trip || trip.empty()) {
        matches.push_back(_block);
      }
    } catch (...) {continue;} 
  }
  return matches;
}

void Tree::declare_user(user user_, std::string nick) {
    json j; 
    j["d"] = user_.trip;
    j["keys"]["sig_pubk"] = user_.pubkeys.DSA;
    j["keys"]["enc_pubk"] = user_.pubkeys.RSA;

    if (!nick.empty()) {
        j["n"] = nick;
    }
    std::string sig = cDSA::sign(user_.prikeys.DSA, j.dump());
    
    json f;
    f["cont"] = j;
    f["sig"] = sig;

    this->gen_block(f.dump(), j["d"]);
}

bool Tree::verify_chain() {
    std::map<std::string, bool> s_trip_seen;
    for (const auto& [_hash, _block] : this->get_chain()) {
        // make sure every hash is valid.
        if (!_block.verify(this->pow)) return false;
        /**
        * If there were multiple server roots, we couldn't tell which was valid.
        * P2P prevents the propagation of new roots, but this means we have
        * to fail if there's more than one root
        */
        bool server_connected = false;
        for (const auto& ph : _block.p_hashes) {
            if (get_chain().find(ph) == get_chain().end()) return false; //parent hashes all need to exist in the chain
            if (get_chain()[ph].s_trip == _block.s_trip) server_connected = true;
        }
        if (!server_connected) {
            s_trip_seen[_block.s_trip] = !s_trip_seen[_block.s_trip]; //first will make true, second back to false
            if (!s_trip_seen[_block.s_trip]) return false;
            // if it's false (was true before, i.e. was flipped, so this is the second sighting) we fail
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

bool Tree::is_intraserver_childless(block to_check) {
    std::string server_trip = to_check.s_trip;
    for (const auto& ch : to_check.c_hashes) {
        if (get_chain()[ch].s_trip == server_trip) return false;
    }
    return true;
}

bool Tree::is_intraserver_orphan(block to_check) {
    std::string server_trip = to_check.s_trip;
    for (const auto& ch : to_check.p_hashes) {
        if (get_chain()[ch].s_trip == server_trip) return false;
    }
    return true;
}

int Tree::intraserver_child_count(block to_check) {
    int result = 0;
    std::string server_trip = to_check.s_trip;
    for (const auto& ch : to_check.c_hashes) {
        if (get_chain()[ch].s_trip == server_trip) result++;
    }
    return result;
}

int Tree::intraserver_parent_count(block to_check) {
    int result = 0;
    std::string server_trip = to_check.s_trip;
    for (const auto& ch : to_check.p_hashes) {
        if (get_chain()[ch].s_trip == server_trip) result++;
    }
    return result;
}

std::unordered_set<std::string> Tree::get_qualifying_hashes(std::function<bool(Tree*, block)> qual_func, std::string s_trip) {
    std::unordered_set<std::string> qualifying_hashes;
    for (const auto& [hash, block] : get_chain()) {
        if (s_trip != "" && block.s_trip != s_trip) continue;
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
    if ((this->add_block_funcs).contains(to_push.s_trip)) {
        (this->add_block_funcs)[to_push.s_trip](to_push.hash);
    }
}

void Tree::batch_push(std::vector<block> to_push_set, bool save_new) {
    std::map<std::string, std::unordered_set<std::string>> server_hash_sections;
    for (const auto& to_push_block : to_push_set) {
        (this->chain)[to_push_block.hash] = to_push_block;
        server_hash_sections[to_push_block.s_trip].insert(to_push_block.hash);
    }
    for (const auto& [hash, block] : get_chain()) {
        link_block(block);
    }
    if (save_new) {
        for (const auto& pushed_block : to_push_set) {
            save(pushed_block);
        }
    }
    for (const auto& [s_trip, section] : server_hash_sections) {
        if (!(this->batch_add_funcs).contains(s_trip)) continue;
        (this->batch_add_funcs)[s_trip](section);
    }
}

void Tree::link_block(block to_link) {
    // ensure that all of the parents are in the chain
    bool has_missing_parents = false;
    for (const auto& ph : to_link.p_hashes) {
        if (get_chain().count(ph) == 0) has_missing_parents = true;
    }
    // three cases where a block has to be purged:
    // - at least one missing parent
    // - no parents & chain already has a root
    // - no intraserver parents & server already has a root
    // first one means the hash won't match the parent set, second and third would enable multiple roots
    if (
        has_missing_parents ||
        (is_orphan(to_link) && (this->has_root)) ||
        (is_intraserver_orphan(to_link) && (this->rooted_servers).contains(to_link.s_trip))
    ) {
        recursive_cleanse(to_link.hash);
        return;
    }

    // add child hashes - derived from parent hashes, just a convenience
    for (const auto& ph : to_link.p_hashes) {
        (this->chain)[ph].c_hashes.insert(to_link.hash);
    }
    
    //update whether chain/server is rooted.
    if (is_orphan(to_link) && !(this->has_root)) 
        this->has_root = true;

    if (is_intraserver_orphan(to_link) && !(this->rooted_servers).contains(to_link.s_trip)) 
        (this->rooted_servers).insert(to_link.s_trip);
}

void Tree::recursive_cleanse(std::string target) {
    if (get_chain().count(target) != 0) {
        (this->chain).erase(get_chain()[target].hash);
        for (const auto& ch : get_chain()[target].c_hashes) {
            recursive_cleanse(ch);
        }
    }
}

void Tree::save(block to_save) {
    if (!dir_linked) return;
    std::string block_string = to_save.dump();
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

