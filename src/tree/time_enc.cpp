#include <chrono>
#include <ctime>
#include <cstring>

#include <string>
#include "../../inc/tree.h"

using namespace std::chrono;

unsigned long long get_raw_time() {
    system_clock::time_point tp = system_clock::now();
    system_clock::duration dtn = tp.time_since_epoch();
    return dtn.count();
}

std::string raw_time_to_string(unsigned long long raw_time) {
    unsigned char buf[8];
    std::memcpy(buf, &raw_time, 8);
    std::string output(buf, buf + sizeof(buf));
    return output;
}

unsigned long long string_to_raw_time(std::string str_time) {
    unsigned long long output;
    memcpy(&output, str_time.c_str(), 8);
    return output;
}