#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <array>
#include <compare>
#include <filesystem>
#include <queue>
#include <nlohmann/json.hpp>
#include <errno.h>
#include <atomic>
#include <mutex>
#include <cassert>

#include "crypt.hpp"
#include "strops.hpp"
#include "chain.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>

using json = nlohmann::json;

#pragma once

unsigned long long get_raw_time();

std::string raw_time_to_string(unsigned long long raw_time);

unsigned long long string_to_raw_time(std::string str_time);

struct block {
    /* data */
    unsigned long long time;
    std::string nonce;
    std::string s_trip;
    std::string c_trip;
    std::string cont;
    std::string hash;
    std::unordered_set<std::string> p_hashes;

    /* utility */
    std::string hash_concat() const;
    bool verify(int pow = 0) const;
    json jdump() const;
    std::string dump() const;
    
    /* vertex */
    std::string trip();
    std::unordered_set<std::string> p_trips();

    /** construct */
    block();
    block(json origin);
    block(
        std::string cont, 
        std::unordered_set<std::string> p_hashes, 
        int pow, 
        std::string s_trip, 
        unsigned long long set_time = get_raw_time(), 
        std::string c_trip = ""
        );
};

// block hashes are taken on faith to save time
// verification occurs only when blocks are added to a tree

namespace std {
    template<> struct hash<block>
    {
        std::size_t operator()(const block& b) const noexcept
        {
            return std::hash<std::string>{}(b.hash);
        }
    };
}

bool operator== (block a, block b) {
    return (a.hash == b.hash);
}

std::vector<std::string> order_hashes(std::unordered_set<std::string> input_hashes);

class Tree : public chain_model<block> {
    protected:
        std::string target_dir;

        int pow = 0;

        bool dir_linked = false;

        std::unordered_set<std::string> saved_hashes;

        std::map<std::string, linked<block>*> server_roots;

        void save(block to_save);

        void chain_configure(block root) override;

        std::unordered_set<block> get_valid(std::unordered_set<block> to_check) override;

        void push_response(std::unordered_set<std::string> new_trips, std::unordered_set<std::string> flags = std::unordered_set<std::string>()) override;
    public:
        std::map<std::string, std::function<void(std::unordered_set<std::string>)>> server_add_funcs;

        Tree();

        Tree(std::string dir);

        void load(std::string dir);

        void set_pow_req(int pow_req);

        int get_pow_req();

        std::string gen_block(std::string cont, std::string s_trip, unsigned long long set_time = get_raw_time(), std::unordered_set<std::string> p_hashes = std::unordered_set<std::string>(), std::string c_trip = std::string(24, '='));

        std::unordered_set<std::string> find_p_hashes(std::string s_trip, std::unordered_set<std::string> base_p_hashes = std::unordered_set<std::string>(), int p_count = 3);

        bool is_childless(std::string to_check);

        bool is_orphan(std::string to_check);

        bool is_intraserver_childless(std::string to_check);

        bool is_intraserver_orphan(std::string to_check);

        std::unordered_set<std::string> intraserver_c_hashes(std::string to_check);

        std::unordered_set<std::string> intraserver_p_hashes(std::string to_check);

        std::unordered_set<std::string> get_qualifying_hashes(std::function<bool(Tree*, std::string)> qual_func, std::string s_trip = std::string());

        std::unordered_set<std::string> get_parent_hash_union(std::unordered_set<std::string> c_hashes);

        void create_root();
};
