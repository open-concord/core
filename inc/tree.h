#include <vector>
#include <array>
#include "miner.h"

#pragma once

class Tree {
    private:
        //int pow;
        //Miner local_miner;
    public:
        std::vector<std::vector<std::string>> local_chain;
        //Tree(int pow_min);

        // where h1 is the new content, and h0 is prev hash
        void generate_branch(bool debug_info, Miner& local_miner, std::string c1, std::string st);

        std::vector<std::vector<std::string>> get_chain();

        bool verify_block(std::vector<std::string> block, int pow_min = 0);

        bool verify_chain(int pow_min = 0);
};

class FileTree {
    public:
        Tree target_tree;
        std::string target_dir;

        FileTree(std::string dir);

        //Pass through normal classes.

        void generate_branch(bool debug_info, Miner& local_miner, std::string c1, std::string st);

        std::vector<std::vector<std::string>> get_chain();
        
        bool verify_block(std::vector<std::string> block, int pow_min = 0);

        bool verify_chain(int pow_min = 0);
};