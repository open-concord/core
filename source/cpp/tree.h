#include <vector>
#include <array>

#pragma once

class Tree {
    private:
        //int pow;
        //Miner local_miner;
    public:
        std::vector<std::array<std::string, 5>> localChain;
        //Tree(int pow_min);

        // where h1 is the new content, and h0 is prev hash
        void generate_branch(bool debug_info, Miner& local_miner, std::string c1);

        // where h0 and h1 are child nodes, and h01 is block to be checked
        bool verify_integrity(std::string h0, std::string h1, std::string h01);
};