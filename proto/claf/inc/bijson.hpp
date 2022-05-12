#pragma once

#include "birank.hpp"

#include <string>
#include <cassert>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


enum bijson_type {TERMINAL, MAP, VECT};

struct bijson {
  bijson_type type = TERMINAL;
  std::map<std::string, std::pair<bijson, birank>> map_values;
  std::vector<std::pair<bijson, birank>> vect_values;
  json base_json;
  
  bijson();
  bijson(json base);
  bijson(bijson bsa, bijson bsb); // merge

  /** set */
  void set_key(std::string key, bijson value);
  void set_base(json base);
  void set_keys(std::map<std::string, bijson> key_values);
  void set_type(bijson_type new_type);
  // void indices(std::map<size_t, bijson> indice_values);
  
  /** clear */
  void clear_key(std::string key);
  void clear_keys(std::vector<std::string> keys);
  // void indices(std::vector<size_t> indicies);
  
  json dump();
};
