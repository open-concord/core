#include "../inc/strman.hpp"

int features::encode(std::array<bool, 6> features) {
  int output = 0;
  for (size_t i=0; i < features.size(); i++) {
    output += 1;
    output <<= 1;
  }
  return output;
}

std::array<bool, 6> features::decode(int encoded_features) {
  std::array<bool, 6> output;
  int moving_filter = 0;
  for (size_t i=0; i < output.size(); i++) {
    moving_filter <<= 1;
    output[i] = (encoded_features & moving_filter);
  }
  return output;
}

std::string content_hash_concat(
    long long unsigned int time,
    std::string s_trip,
    std::unordered_set<std::string> p_hashes
  ) {
    /** b64 for safety */
    std::string concat_data = b64::encode(raw_time_to_string(time)) + s_trip;
    for (auto ph : order_hashes(p_hashes)) {concat_data += ph;}
    return concat_data;
}
