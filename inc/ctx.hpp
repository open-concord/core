#pragma once
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <unordered_set>

#include "tree.hpp"

namespace Ctx {
  struct Exchange {
    bool complete = false;
    std::unordered_set<block> NewBlocks;
    std::string ChainTrip;
    Tree* CurrentTree;
  };
  struct Graph {
    std::map<std::string, Tree*> Forest;
    bool filter, blacklist = false;
    std::deque<std::string> filtered_trees;
  };
}
