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

using json = nlohmann::json;

#pragma once

bool no_filter(json data);

bool type_filter(char qtype, json data);

json get_continuity_value(std::vector<json> defs,  std::string key);

std::string chain_encrypt(json data, std::string dsa_pri_key, std::string rsa_pub_key, std::string aes_key, char mt);

unsigned long long get_raw_time();

std::string raw_time_to_string(unsigned long long raw_time);

unsigned long long string_to_raw_time(std::string str_time);

struct block {
    unsigned long long time;
    std::string nonce;
    std::string s_trip;
    std::string c_trip;
    std::string cont;
    std::string hash;
    std::unordered_set<std::string> p_hashes;
    //these properties are for chain analysis and aren't actually saved
    std::unordered_set<std::string> c_hashes;
    /** state */
    std::string hash_concat() const;
    bool verify(int pow = 0) const;
    json jdump() const;
    std::string dump() const;
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

struct keypair {
    std::string DSA;
    std::string RSA;

    keypair() {}
    keypair(std::string dsak, std::string rsak) : DSA(dsak), RSA(rsak) {}
};

struct user {
    std::string trip;
    keypair pubkeys, prikeys;
    // bool empty = false;

    user() {
        std::array<std::string, 2> DSA = cDSA::keygen();
        std::array<std::string, 2> RSA = cRSA::keygen();

        pubkeys = keypair(b64::encode(DSA[1]), b64::encode(RSA[1]));
        prikeys = keypair(b64::encode(DSA[0]), b64::encode(RSA[0]));

        this->trip = gen::trip(pubkeys.DSA + pubkeys.RSA, 24);
    }

    user(keypair pubset) : pubkeys(pubset) {
        this->trip = gen::trip(pubset.DSA + pubset.RSA, 24);
    }

    user(keypair pubset, keypair priset) : pubkeys(pubset), prikeys(priset) {
        this->trip = gen::trip(pubset.DSA + pubset.RSA, 24);
    }

    user(std::string trip, keypair priset) : trip(trip), prikeys(priset) {}
};

class Tree {
    private:
        //int pow;
        std::map<std::string, block> chain;

        std::string target_dir;

        int pow = 0;

        bool dir_linked = false;

        bool push_paused = false;
        
        std::atomic<bool> push_proc_active = false;

        std::mutex push_proc_mtx;

        std::string chain_root;

        std::map<std::string, std::string> server_roots;


        std::queue<std::pair<std::unordered_set<block>, bool>> awaiting_push_batches;
        //pairs are <new blocks, whether to save>

        void chain_configure(block root);

        void save(block to_save);

        void link_block(std::string to_link);

        void recursive_purge(std::string target);

        void batch_push(std::unordered_set<block> to_push, bool save_new = true);

        void queue_batch(std::pair<std::unordered_set<block>, bool> to_queue);

        void push_proc();
    public:
        std::map<std::string, std::function<void(std::string)>> add_block_funcs;
        std::map<std::string, std::function<void(std::unordered_set<std::string>)>> batch_add_funcs;

        Tree();

        Tree(std::string dir);

        void load(std::string dir);

        void set_pow_req(int pow_req);

        int get_pow_req();

        std::string gen_block(std::string cont, std::string s_trip, unsigned long long set_time = get_raw_time(), std::unordered_set<std::string> p_hashes = std::unordered_set<std::string>(), std::string c_trip = std::string(24, '='));

        std::unordered_set<std::string> find_p_hashes(std::string s_trip, std::unordered_set<std::string> base_p_hashes = std::unordered_set<std::string>(), int p_count = 3);

        std::map<std::string, block> get_chain();

        bool is_childless(std::string to_check);

        bool is_orphan(std::string to_check);

        bool is_intraserver_childless(std::string to_check);

        bool is_intraserver_orphan(std::string to_check);

        std::unordered_set<std::string> intraserver_c_hashes(std::string to_check);

        std::unordered_set<std::string> intraserver_p_hashes(std::string to_check);

        std::unordered_set<std::string> get_qualifying_hashes(std::function<bool(Tree*, std::string)> qual_func, std::string s_trip = std::string());

        std::unordered_set<std::string> get_parent_hash_union(std::unordered_set<std::string> c_hashes);

        std::unordered_set<block> search_user(std::string trip = "");

        void declare_user(user user_, std::string nick = "");

        //std::vector<json> search(char message_type, std::string target_trip, std::string key, boost::function<bool(json)> filter = no_filter, int start_b = -1, int end_b = -1);

        bool verify_chain();
        
        void unit_push(block to_push, bool save_new = true);

        void set_push(std::unordered_set<block> to_push, bool save_new = true);

        void set_push(std::vector<block> to_push, bool save_new = true);
};
