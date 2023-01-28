#include <chrono>
#include <ctime>
#include <cstring>

#include <string>
#include "../../inc/tree.hpp"

using namespace std::chrono;

unsigned long long timeh::raw() {
  system_clock::time_point tp = system_clock::now();
  system_clock::duration dtn = tp.time_since_epoch();
  return dtn.count();
}

std::string timeh::to_string(unsigned long long raw_time) {
  unsigned char buf[8];
  std::memcpy(buf, &raw_time, 8);
  return std::string(buf, buf + sizeof(buf));
}

unsigned long long timeh::from_string(std::string str_time) {
  unsigned long long output;
  std::memcpy(&output, str_time.c_str(), 8);
  return output;
}
