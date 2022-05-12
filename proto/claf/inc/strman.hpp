#pragma once
#include "claf.hpp"

std::string content_hash_concat(
    long long unsigned int time, 
    std::string s_trip, 
    std::unordered_set<std::string> p_hashes
);

namespace features {
  int encode(std::array<bool, 6>);
  std::array<bool, 6> decode(int);
}
