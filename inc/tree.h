#include <vector>
#include <array>
#include "miner.h"

#pragma once

class Tree {
    private:
        //int pow;
        std::vector<std::vector<std::string>> local_chain;
        int pow = 0;
    public:
        void set_pow_req(int pow_req);

        // where h1 is the new content, and h0 is prev hash
        void generate_branch(bool debug_info, std::string c1, std::string st);

        std::vector<std::vector<std::string>> get_chain();

        bool verify_chain();

        void chain_push(std::vector<std::string> block);
};

class FileTree {
    private:
    public:
        Tree target_tree;
        std::string target_dir;

        FileTree();
        FileTree(std::string dir);
        void load(std::string dir);

        void set_pow_req(int POW_req);

        //Pass through normal classes.

        void generate_branch(bool debug_info, std::string c1, std::string st);

        std::vector<std::vector<std::string>> get_chain();

        bool verify_chain();

        void chain_push(std::vector<std::string> block);
        
        void save_latest();
};

bool verify_block(std::vector<std::string> block, int pow);