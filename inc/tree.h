#include <string>
#include <vector>
#include <map>
#include <array>
#include <boost/function.hpp>
#include <nlohmann/json.hpp>

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
    std::vector<std::string> p_hashes;
    std::vector<std::string> c_hashes; //this property is for chain analysis and isn't actually saved
};

bool verify_block(block to_verify, int pow);

json block_to_json(block input);

block json_to_block(json input);

block construct_block(std::string cont, std::vector<std::string> p_hashes, int pow, std::string s_trip, std::string c_trip = std::string(24, '='));

class Tree {
    private:
        //int pow;
        std::map<std::string, block> chain;

        std::string target_dir;

        int pow = 0;
        bool dir_linked = false;

        void save(block to_save);
        
        void link_block(block to_link);
    public:
        Tree();
        
        Tree(std::string dir);

        void load(std::string dir);

        void set_pow_req(int pow_req);

        void gen_block(std::string cont, std::string s_trip, int p_count = 2, std::string c_trip = std::string(24, '='));

        std::map<std::string, block> get_chain();

        //std::vector<json> search(char message_type, std::string target_trip, std::string key, boost::function<bool(json)> filter = no_filter, int start_b = -1, int end_b = -1);

        bool verify_chain();

        void chain_push(block to_push);
};