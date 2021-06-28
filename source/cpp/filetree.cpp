#include "tree.h"
#include "utils.h"
#include <array>
#include <fstream>
#include <iostream>

FileTree::FileTree(std::string dir) {
    this->target_dir = dir;
    if ((this->target_dir).back() != '/') this->target_dir += "/";
    for (size_t i = 0; true; i++) {
        std::ifstream saved_block((this->target_dir) + std::to_string(i) + ".block");
        if (saved_block) {
            std::string block_data;
            saved_block.seekg(0, std::ios::end); //move to stream end
            block_data.resize(saved_block.tellg()); //expand string based on stream end position
            saved_block.seekg(0, std::ios::beg);
            saved_block.read(&block_data[0], block_data.size());
            std::cout << block_data.length() << std:: endl;
            (this->target_tree).local_chain.push_back({
                block_data.substr(0, 22), //22 chars of datetime
                block_data.substr(22, 64), //64 chars of last hash
                block_data.substr(22 + 64, 64), //64 chars of current hash
                block_data.substr(22 + 64 + 64, 32), //32 chars of nonce
                block_data.substr(22 + 64 + 64 + 32) //remainder is plain content
            });
            saved_block.close();
        }
        else break;
    }
}

void FileTree::generate_branch(bool debug_info, Miner& local_miner, std::string c1) {
    (this->target_tree).generate_branch(debug_info, local_miner, c1);
    size_t terminal_index = (this->target_tree).get_chain().size() - 1;
    std::array<std::string, 5> new_block = (this->target_tree).get_chain()[terminal_index];
    std::string block_string;
    for (size_t i = 0; i < 5; i++) block_string+=new_block[i];
    std::ofstream block_file(((this->target_dir) + std::to_string(terminal_index) + ".block").c_str());
    block_file << block_string;
    block_file.close();
}

bool FileTree::verify_integrity(std::string h0, std::string h1, std::string h01) {
    return (this->target_tree).verify_integrity(h0, h1, h01);
}

std::vector<std::array<std::string, 5>> FileTree::get_chain() {
    return (this->target_tree).get_chain();
}