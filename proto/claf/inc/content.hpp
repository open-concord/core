#pragma once
#include "claf.hpp"

#include <string>
#include <map>
#include <array>

struct member {
  std::string user_trip;
  std::map<std::string, birank> roles_ranks;
};

struct role {
  std::array<birank, 6> features;
  std::array<unsigned int, 2> primacy_rank;
  /** get */
  bool get_primacy() {return primacy_rank[0];}
  bool get_mute() {return features[0];}
  
  /** can */
  bool can_invite() {return features[1];}
  bool can_rem() {return features[2];}
  bool can_rgrant() {return features[3];}
  bool can_rcreate() {return features[4];}
  bool can_edit() {return features[5];}
};

struct message {
  std::string hash;
  char supertype;
  std::string type;
  json data, extra;
};
