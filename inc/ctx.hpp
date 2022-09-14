#pragma once
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <unordered_set>

#include "tree.hpp"
#include <debug.hpp>

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
