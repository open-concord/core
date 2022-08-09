#include "../../inc/tree.hpp"
#include <mutex>

Tree::Tree(std::string dir) {
    load(dir);
}

void Tree::load(std::string dir) {
    this->dir_linked = true;
    this->target_dir = dir;
    if ((this->target_dir).back() != '/') this->target_dir += "/";
    
    struct stat info;
    /** right now, this is hardcoded file dir.
     * TODO: Allow FIFO, socket, etc.
     */ 
    if ((stat( (this->target_dir).c_str(), &info ) != 0) || (info.st_mode & S_IFMT) != S_IFDIR) {
        int err = errno;
        if (err) {std::cout << "[!] errno: " << err << '\n';}
        throw std::invalid_argument("Directory is not accessible.");
    }

    std::filesystem::path p(this->target_dir);

    std::unordered_set<block> loaded_blocks;

    for(auto& entry : std::filesystem::directory_iterator(p)) {
        std::string path_str = entry.path().string();
        if (path_str.substr(path_str.length() - 6) != ".block") continue; // only want .block files
        std::ifstream saved_block(path_str.c_str());
        if (saved_block) {
            std::string block_data;
            saved_block.seekg(0, std::ios::end); // move to stream end
            block_data.resize(saved_block.tellg()); // expand string based on stream end position
            saved_block.seekg(0, std::ios::beg);
            saved_block.read(&block_data[0], block_data.size());
            saved_block.close();
            block parsed_block(json::parse(block_data));
            loaded_blocks.insert(parsed_block);
        }
        else continue;
    }
    std::lock_guard lk(this->push_proc_mtx);
    batch_push(loaded_blocks, std::unordered_set<std::string>({"no-save"}));
}

void Tree::save(block to_save) {
    if (!dir_linked) return;
    std::string block_string = to_save.dump();
    std::ofstream block_file(((this->target_dir) + to_save.hash + ".block").c_str());
    block_file << block_string;
    block_file.close();
}
