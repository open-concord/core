#include "../../inc/server.h"
#include "../../inc/tree.h"
#include <string>
#include <set>

std::string content_hash_concat(std::string time, std::string s_trip, std::unordered_set<std::string> p_hashes) {
    std::string concat_data = b64_encode(raw_time_to_string(time)) + s_trip; //b64 timestr encoding is only for safety
    for (auto ph : order_hashes(p_hashes)) concat_data += ph;
    return concat_data;
}