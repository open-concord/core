#include "../../inc/ftree.hpp"
#include <mutex>

FileTree::
FileTree(std::string dir) {
  load(dir);
}

void
FileTree::load(std::string dir) { 
  this->dir = dir;

  if ((this->dir).back() != '/') this->dir += "/";
  
  std::filesystem::path p(this->dir);

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
      } else continue;
  }
  
  std::lock_guard lk(this->push_proc_mtx);
  batch_push(loaded_blocks, std::unordered_set<std::string>({"no-save"}));
}

void
FileTree::save(block to_save) { 
  std::string block_string = to_save.dump();
  std::ofstream block_file(((this->dir) + to_save.hash + ".block").c_str());
  block_file << block_string;
  block_file.close();
}

void
FileTree::apply(std::unordered_set<std::string> paths) {
  std::unordered_set<block> to_check;
  for (const auto& path : paths) { // read blocks
    std::ifstream f(path);
    to_check.insert(block(json::parse(f)));
  }
  std::unordered_set<block> valid = get_valid(to_check);
  queue_batch(valid);
  // TODO Either loop over valid blocks, or overload push_reponses to take a set of blocks
}
