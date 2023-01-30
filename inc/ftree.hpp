/** 
 * \addtogroup Core
 * \{ 
 */
#pragma once

#include "tree.hpp"

/**
 * \brief Filesystem extension of Tree
 */
class FileTree : public Tree {
protected:
  /**
   * \brief FileTree's monitored directory.
   */
  std::string dir;
  
  /**
   * \brief Write block to FileTree::target_dir
   * \param to_save Block to save
   */
  void save(block to_save) override;

public: 
  /**
   * \brief Loads a file descriptor for storage.
   * \param dir Directory to target
   */
  void load(std::string dir);

  /**
   * \brief Functionally equivalant to FileTree::load.
   * \param dir Directory to monitor.
   */
  FileTree(std::string fpath);
  
  ~FileTree();
};
/** \} */
