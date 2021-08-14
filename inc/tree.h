#include <vector>
#include <array>
#include <boost/function.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#pragma once

bool no_filter(json data);

bool type_filter(char qtype, json data);

json get_continuity_value(std::vector<json> defs,  std::string key);

std::string chain_encrypt(json data, std::string dsa_pri_key, std::string rsa_pub_key, std::string aes_key, char mt);

bool verify_block(std::vector<std::string> block, int pow);

class Tree {
    private:
        //int pow;
        std::vector<std::vector<std::string>> local_chain;

        std::string target_dir;

        int pow = 0;
        bool dir_linked = false;
    public:
        Tree();
        
        Tree(std::string dir);

        void load(std::string dir);

        void set_pow_req(int pow_req);

        // where h1 is the new content, and h0 is prev hash
        void generate_branch(bool debug_info, std::string c1, std::string st, std::string creator_trip = std::string(24, '='));

        std::vector<std::vector<std::string>> get_chain();

        std::vector<json> search(char message_type, std::string target_trip, std::string key, boost::function<bool(json)> filter = no_filter, int start_b = -1, int end_b = -1);

        bool verify_chain();

        void chain_push(std::vector<std::string> block);

        void save_latest();
};