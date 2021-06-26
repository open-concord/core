#pragma once

class Tree {
    private:
        //int pow;
        //Miner local_miner;
    public:
        //Tree(int pow_min);

        // where h1 is the new content, and h0 is prev hash
        std::string generate_branch(bool debug_info, Miner& local_miner, std::string c1, std::string h0);

        // where h0 and h1 are child nodes, and h01 is block to be checked
        bool verify_integrity(std::string h0, std::string h1, std::string h01);
};