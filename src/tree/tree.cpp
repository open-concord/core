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
#include <cassert>

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

// block pushing
//---

// the internal push function
void Tree::batch_push(std::unordered_set<block> to_push_set, bool save_new) {
    std::map<std::string, std::unordered_set<std::string>> server_hash_sections;
    
    //add all blocks
    for (const auto& to_push_block : to_push_set) {
        //reject blocks that are
        // - already included 
        // - invalid
        if (!to_push_block.verify(this->pow) || get_chain().contains(to_push_block.hash)) {
            to_push_set.erase(to_push_block);
            continue;
        }
        //otherwise, add the block to the chain - it will be removed during linking if it doesn't connect properly.
        (this->chain)[to_push_block.hash] = to_push_block;
    }

    //link new blocks after *all* have been added.
    for (const auto& to_push_block : to_push_set) {
        link_block(to_push_block.hash);
    }

    //go through blocks that were successfully linked to save and trigger callbacks.
    for (const auto& to_push_block : to_push_set) {
        //check that block has made it through linking
        if (!get_chain().contains(to_push_block.hash)) continue;
        //save if applicable
        if (save_new) save(to_push_block);
        //add hash to batch for server callback
        server_hash_sections[to_push_block.s_trip].insert(to_push_block.hash);
    }
    
    //trigger server callbacks with collected hashes
    for (const auto& [s_trip, section] : server_hash_sections) {
        if ((this->batch_add_funcs).contains(s_trip))
            (this->batch_add_funcs)[s_trip](section);
    }
}

//add a block to the queue, ensuring a winner (i.e. processing is only called once)
void Tree::queue_batch(std::pair<std::unordered_set<block>, bool> to_queue) {
    (this->awaiting_push_batches).push(to_queue);
    (this->push_proc_mtx).lock();
    if (!push_proc_active) {
        push_proc_active = true;
        (this->push_proc_mtx).unlock();
        push_proc();
    } else {
        (this->push_proc_mtx).unlock();
    }
}

void Tree::push_proc() {
    while (true) {
        std::pair<std::unordered_set<block>, bool> next_batch;
        {
            std::lock_guard<std::mutex> lk(this->push_proc_mtx);
            if ((this->awaiting_push_batches).empty()) {
                this->push_proc_active = false;
                return;
            }
            next_batch = awaiting_push_batches.front();
            (this->awaiting_push_batches).pop();
        }
        batch_push(next_batch.first, next_batch.second);
    }
}

// user-facing options
void Tree::unit_push(block to_push, bool save_new) {
    std::pair<std::unordered_set<block>, bool> unit_batch;
    unit_batch.first.insert(to_push);
    unit_batch.second = save_new;
    queue_batch(unit_batch);
}

void Tree::set_push(std::unordered_set<block> to_push, bool save_new) {
    std::pair<std::unordered_set<block>, bool> set_batch;
    set_batch.first = to_push;
    set_batch.second = save_new;
    queue_batch(set_batch);
}

void Tree::set_push(std::vector<block> to_push, bool save_new) {
    set_push(std::unordered_set<block>(to_push.begin(), to_push.end(), to_push.size()), save_new);
}
// end of push functions
//---


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

    std::unordered_set<block> loaded_blocks;

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
            loaded_blocks.insert(parsed_block);
        }
        else continue;
    }
    set_push(loaded_blocks, false);
}

void Tree::chain_configure(block root) {
    //for now, just extract POW threshold
    json config = json::parse(root.cont);
    if (config.contains("pow")) {
        set_pow_req((int) config["pow"]);
    }
}

void Tree::set_pow_req(int POW_req) {
    bool upward_change = (this->pow) < POW_req;
    this->pow = POW_req;
    if (upward_change) {
        //blocks could be made invalid, so we need to re-link the entire tree.
        for (const auto& [hash, block] : (this->chain)) {
            link_block(hash);
        }
    }
}

int Tree::get_pow_req() {
    return this->pow;
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
    unit_push(out_block);
    return out_block.hash;
}

std::unordered_set<std::string> Tree::find_p_hashes(std::string s_trip, std::unordered_set<std::string> base_p_hashes, int p_count) {
    /**
    * there's no point in using blocks with existing children as hashes;
    * we can get the same reliance by using their children
    */
    std::unordered_set<std::string> p_hashes = base_p_hashes;

    std::unordered_set<std::string> intra_childless_hashes = get_qualifying_hashes(&Tree::is_childless);

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
        std::max(p_count - (int) p_hashes.size(), (int) require_intra_block), //we want at least one block from the server for continuity, but if we already have one we just need to fill the p_count.
        std::mt19937{std::random_device{}()}
    );
    int p_remainder = p_count - (int) p_hashes.size();
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

bool Tree::is_childless(std::string to_check) {
    return (get_chain()[to_check].c_hashes.size() == 0);
}

bool Tree::is_orphan(std::string to_check) {
    return (get_chain()[to_check].p_hashes.size() == 0);
}

bool Tree::is_intraserver_childless(std::string to_check) {
    block tc_block = get_chain()[to_check];
    std::string server_trip = tc_block.s_trip;
    for (const auto& ch : tc_block.c_hashes) {
        if (get_chain()[ch].s_trip == server_trip) return false;
    }
    return true;
}

bool Tree::is_intraserver_orphan(std::string to_check) {
    block tc_block = get_chain()[to_check];
    std::string server_trip = tc_block.s_trip;
    for (const auto& ch : tc_block.p_hashes) {
        if (get_chain()[ch].s_trip == server_trip) return false;
    }
    return true;
}


std::unordered_set<std::string> Tree::intraserver_c_hashes(std::string to_check) {
    block tc_block = get_chain()[to_check];
    std::unordered_set<std::string> result;
    std::string server_trip = tc_block.s_trip;
    for (const auto& ch : tc_block.c_hashes) {
        if (get_chain()[ch].s_trip == server_trip) result.insert(ch);
    }
    return result;
}

std::unordered_set<std::string> Tree::intraserver_p_hashes(std::string to_check) {
    block tc_block = get_chain()[to_check];
    std::unordered_set<std::string> result;
    std::string server_trip = tc_block.s_trip;
    for (const auto& ph : tc_block.p_hashes) {
        if (get_chain()[ph].s_trip == server_trip) result.insert(ph);
    }
    return result;
}

std::unordered_set<std::string> Tree::get_qualifying_hashes(std::function<bool(Tree*, std::string)> qual_func, std::string s_trip) {
    std::unordered_set<std::string> qualifying_hashes;
    for (const auto& [hash, block] : get_chain()) {
        if (!s_trip.empty() && block.s_trip != s_trip) continue;
        if (qual_func(this, hash)) qualifying_hashes.insert(hash);
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

void Tree::create_root() {
    assert(this->get_chain().size() == 0);
    json root_msg;
    root_msg["pow"] = this->pow;
    this->gen_block(root_msg.dump(), std::string(24, '='));
}

void Tree::link_block(std::string to_link) {
    if (!get_chain().contains(to_link)) return;
    block tl_block = get_chain()[to_link];
    // ensure that all of the parents are in the chain
    bool has_missing_parents = false;
    for (const auto& ph : tl_block.p_hashes) {
        if (!get_chain().contains(ph)) has_missing_parents = true;
    }

    // four cases where a block has to be purged:
    // - at least one missing parent
    // = invalid (hash is a mismatch for contents)
    // - no parents & chain already has a different root
    // - no intraserver parents & server already has a different root
    // first one means the hash won't match the parent set, 
    // second ensures that all hashes are properly assigned,
    // third and fourth prevent multiple roots
    if (
        has_missing_parents ||
        !tl_block.verify(this->pow)||
        (is_orphan(to_link) && !(this->chain_root).empty() && this->chain_root != to_link) ||
        (is_intraserver_orphan(to_link) && !(this->server_roots[tl_block.s_trip]).empty() && this->server_roots[tl_block.s_trip] != to_link)
        //server_roots[tO_link.s_trip] == ... initializes the position, but .contains is checked first, so that won't be evaluated if it doesn't contain the s_trip
    ) {
        recursive_purge(to_link);
        return;
    }

    // add child hashes - derived from parent hashes, just a convenience
    for (const auto& ph : tl_block.p_hashes) {
        (this->chain)[ph].c_hashes.insert(to_link);
    }
    
    //update whether chain/server is rooted.
    if (is_orphan(to_link) && (this->chain_root).empty()) {
        this->chain_root = to_link;
        chain_configure(tl_block);
    }

    if (is_intraserver_orphan(to_link) && (this->server_roots[tl_block.s_trip]).empty()) 
        this->server_roots[tl_block.s_trip] = to_link;
}

void Tree::recursive_purge(std::string target) {
    if (get_chain().contains(target)) {
        (this->chain).erase(target);
        for (const auto& ch : get_chain()[target].c_hashes) {
            recursive_purge(ch);
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

