#include <string>
#include <vector>
#include <array>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>

#include "../../inc/tree.h"

struct stat info;

FileTree::FileTree() {

}

FileTree::FileTree(std::string dir) {
    load(dir);
}

void FileTree::load(std::string dir) {
    this->target_dir = dir;
    if ((this->target_dir).back() != '/') this->target_dir += "/";

    if ((stat( (this->target_dir).c_str(), &info ) != 0) || !(info.st_mode & S_IFDIR) ) {
        throw std::invalid_argument("Directory is not accessible.");
    }

    for (size_t i = 0; true; i++) {
        std::ifstream saved_block((this->target_dir) + std::to_string(i) + ".block");
        if (saved_block) {
            std::string block_data;
            saved_block.seekg(0, std::ios::end); //move to stream end
            block_data.resize(saved_block.tellg()); //expand string based on stream end position
            saved_block.seekg(0, std::ios::beg);
            saved_block.read(&block_data[0], block_data.size());
            (this->target_tree).chain_push({
                block_data.substr(0, 22), //22 chars of datetime
                block_data.substr(22, 64), //64 chars of last hash
                block_data.substr(22 + 64, 64), //64 chars of current hash
                block_data.substr(22 + 64 + 64, 32), //32 chars of nonce
                block_data.substr(22 + 64 + 64 + 32, 32), //32 chars of server trip
                block_data.substr(22 + 64 + 64 + 32 + 32) //remainder is plain content
            });
            saved_block.close();
        }
        else break;
    }
}

void FileTree::generate_branch(bool debug_info, Miner& local_miner, std::string c1, std::string st) {
    (this->target_tree).generate_branch(debug_info, local_miner, c1, st);
    save_latest();
}

std::vector<std::vector<std::string>> FileTree::get_chain() {
    return (this->target_tree).get_chain();
}

bool FileTree::verify_block(std::vector<std::string> block, int pow_min) {
    return (this->target_tree).verify_block(block, pow_min);
}

bool FileTree::verify_chain(int pow_min) {
    return (this->target_tree).verify_chain(pow_min);
}

void FileTree::chain_push(std::vector<std::string> block) {
    (this->target_tree).chain_push(block);
    save_latest();
}

void FileTree::save_latest() {
    size_t terminal_index = (this->target_tree).get_chain().size() - 1;
    std::vector<std::string> new_block = (this->target_tree).get_chain()[terminal_index];
    std::string block_string;
    for (size_t i = 0; i < 6; i++) block_string+=new_block[i];
    std::ofstream block_file(((this->target_dir) + std::to_string(terminal_index) + ".block").c_str());
    block_file << block_string;
    block_file.close();
}