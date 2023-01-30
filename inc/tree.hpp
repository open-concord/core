/**
 * \addtogroup Core
 * \{
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <array>
#include <compare>
#include <filesystem>
#include <queue>
#include <nlohmann/json.hpp>
#include <errno.h>
#include <atomic>
#include <mutex>
#include <cassert>

#include "crypt.hpp"
#include "strops.hpp"
#include "graph.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>

using json = nlohmann::json;

namespace timeh {
  unsigned long long raw();

  std::string to_string(unsigned long long raw_time);

  unsigned long long from_string(std::string str_time);
};

/**
 * \brief Vertex interpretation for use with Tree
 */
struct block {
    /* data */
    unsigned long long time;
    std::string nonce;
    std::string s_trip;
    std::string c_trip;
    std::string cont;
    std::string hash;
    std::unordered_set<std::string> p_hashes;

    /* utility */
    std::string hash_concat() const;
    bool verify(int pow = 0) const;
    json jdump() const;
    std::string dump() const;
    
    /* vertex */
    std::string trip();
    std::unordered_set<std::string> p_trips();

    /** construct */
    block();
    block(json origin);
    block(
        std::string cont, 
        std::unordered_set<std::string> p_hashes, 
        int pow, 
        std::string s_trip, 
        unsigned long long set_time = timeh::raw(), 
        std::string c_trip = ""
        );
};

/**
 * \brief block hashes are taken on faith to save time, verification occurs only when blocks are added to a tree
*/
namespace std {
  template<> struct hash<block>
  {
    std::size_t operator()(const block& b) const noexcept
    {
      return std::hash<std::string>{}(b.hash);
    }
  };
}

bool operator == (const block x, const block y) {return x.hash == y.hash;}

std::vector<std::string> order_hashes(std::unordered_set<std::string> input_hashes);

/**
 * \brief Default graph interpretation model
 */
class Tree : public graph_model<block> {
protected:
  /**
   * \brief Proof of work requirement.
   *
   * Blocks under this limit will be rejected.
   */
  int pow = 0;

  // FIXME Not sure where we use this -- not referenced anywhere in Tree::
  //std::unordered_set<std::string> saved_hashes;

  /**
   * \brief Referential list of 'server' roots
   * 
   * We maintain an index of 'server' roots so that we can easily ensure that intraserver blocks always reference at least one intraserver parent
   */
  std::map<std::string, linked<block>*> server_roots;

  /**
   * \brief Interprets an established graph
   * \param root Block to interpret as root.
   *
   * Currently only extracts proof of work threshold.
   */
  void graph_configure(block root) override;

  /**
   * \brief Validate a set of blocks
   * \param to_check Set of blocks to validate
   * \returns Set of valid blocks
   */
  std::unordered_set<block> get_valid(std::unordered_set<block> to_check) override;

  /**
   * \brief Apply referential information for a set of blocks
   * \param new_trips Set of trips to index
   * \param flags Set of flags
   */ 
  void push_response(
      std::unordered_set<std::string> new_trips, 
      std::unordered_set<std::string> flags = std::unordered_set<std::string>()
      ) override;
public:
  /**
   * \brief Maps callbacks to server trips
   * 
   * Mapped callbacks are called with new intraserver blocks
   */
  std::map<std::string, std::function<void(std::unordered_set<std::string>)>> server_add_funcs;

  /**
   * \brief Updates a Tree's Proof of Work requirement. 
   * \param pow_req New proof of work requirement.
   *
   * Increasing the proof of work requirement means Tree has to rebuild the entire tree
   */
  void set_pow_req(int pow_req);

  /**
   * \brief Get Tree's proof of work requirement
   * \returns Tree's proof of work requirement
   */
  int get_pow_req();

  /**
   * \brief Save a block to some storage
   * \param block block data 
   */
  virtual void save(block) = 0;

  /**
   * \brief Load an initial storage state
   *
   * Virtual templates aren't allowed as per std, so you'll need to override with the relvant information in your derived Tree types.
   */
  virtual void load() = 0;

  /**
   * \brief Generates a new block and applies to tree. 
   * \param cont String content to be included in the block. This is publicily visible. 
   * \param s_trip Self-identified 'server' trip. Cannot be enforced or verified; used by services to filter content.
   * \param c_trip Optional tripcode of graph-identified user to connect to block.
   * \param set_time The time of block creation. Current time by default.
   * \param p_hashes Parent hashes of blocks. Current valance layer by default.
   * \returns Hash of generated block.
   */
  std::string gen_block(
      std::string cont, 
      std::string s_trip, 
      unsigned long long set_time = timeh::raw(), 
      std::unordered_set<std::string> p_hashes = std::unordered_set<std::string>(), 
      std::string c_trip = std::string{24, '='}
      );

  /**
   * \brief Find intraserver parent hashes
   * \param s_trip 'server' trip
   * \param base_p_hashes Lower-bounded parent hashes
   * \param p_count Number of parents to find
   */
  std::unordered_set<std::string> find_p_hashes(
      std::string s_trip, 
      std::unordered_set<std::string> base_p_hashes = std::unordered_set<std::string>(), 
      int p_count = 3
      );

  /**
   * \brief Determines if a block has children in the Tree
   * \param to_check Hash of the block to check
   * \returns Truth state
   */
  bool is_childless(std::string to_check);
  
  /**
   * \brief Determines if a block has no parents in the Tree
   * \param to_check Hash of the block to check
   * \returns Truth state
   */
  bool is_orphan(std::string to_check);

  /**
   * \brief Determines if a block is childless within the scope of the 'server'
   * \param to_check Hash of the block to check
   * \returns Truth state
   */
  bool is_intraserver_childless(std::string to_check);

  /**
   * \brief Determines if a block has no parents in the 'server'
   * \param to_check Hash of the block to check
   * \returns Truth state
   */
  bool is_intraserver_orphan(std::string to_check);

  /**
   * \brief Returns all children of a given block within the same 'server'
   * \param to_check Hash of source block
   * \returns All found intraserver children
   */
  std::unordered_set<std::string> intraserver_c_hashes(std::string to_check);

  /**
   * \brief Returns all parents of a given block within the same 'server'
   * \param to_check Hash of source block
   * \returns All found intraserver parents
   */
  std::unordered_set<std::string> intraserver_p_hashes(std::string to_check);

  /**
   * \brief Applies some criteria to blocks in the Tree
   * \param qual_func A function pointer to an arbitrary criteria
   * \param s_trip Optional. Only evaluate blocks in a given 'server'
   * \returns Qualifying hashes
   */
  std::unordered_set<std::string> get_qualifying_hashes(
      std::function<bool(Tree*, std::string)> qual_func,
      std::string s_trip = std::string()
      );

  /**
   * \brief Get all parents of given blocks
   * \param c_hashes Hashes of blocks to examine
   * \returns Hashes of found parents
   */
  std::unordered_set<std::string> get_parent_hash_union(std::unordered_set<std::string> c_hashes);

  /**
   * \brief Generate a root block.
   *
   * Only valid in new Trees.
   */
  void create_root();

  Tree();         
};

/**
 * \}
 */
