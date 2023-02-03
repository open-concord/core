/** 
 * \addtogroup Core
 * \{ 
 */
#pragma once

#include "tree.hpp"

/**
 * \brief Filesystem extension of Tree
 *
 * 
 */
class FileTree : public Tree {
protected:
  /**
   * \brief FileTree's storage directory.
   */
  std::string dir;
  
  /**
   * \brief Write block to FileTree::dir
   * \param to_save Block to save
   */
  void save(block to_save) override;

  /**
   * \brief Kernel Queue File Descriptor
   */
  int kqfd = -1;

public: 
  /**
   * \brief Loads a file descriptor for storage.
   * \param dir Directory to target
   */
  void load(std::string dir);
  
  /**
   * \brief Install kernel monitoring on a file descriptor.
   */
  void install(std::string path);
  
  /**
   * \brief Apply a new block
   * \param path Path to new block
   */
  void apply(std::string path); 

  /**
   * \brief Storage directory. Contained blocks are gospel.
   * \param dir Directory to store.
   */
  FileTree(std::string fpath);
  
  ~FileTree();
};
/** \} */
