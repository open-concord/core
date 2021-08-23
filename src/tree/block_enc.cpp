#include <nlohmann/json.hpp>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <string>

#include "tree.h"
#include "strenc.h"

json block_to_json(block input) {
    json output;
    std::string enc_time = b64_encode(raw_time_to_string(input.time));
    std::vector<std::string> v_p_hashes;
    for (const auto& ph : input.p_hashes) {
        v_p_hashes.push_back(ph);
    }
    output["d"] = std::vector<std::string>({enc_time, input.nonce, input.s_trip, input.c_trip, input.cont, input.hash});
    output["p"] = v_p_hashes;
    return output;
}

block json_to_block(json input) {
    block output;
    std::vector<std::string> data = input["d"].get<std::vector<std::string>>();
    output.time = string_to_raw_time(b64_decode(data[0]));
    output.nonce = data[1];
    output.s_trip = data[2];
    output.c_trip = data[3];
    output.cont = data[4];
    output.hash = data[5];
    std::vector<std::string> v_p_hashes = input["p"].get<std::vector<std::string>>();
    for (const auto& ph: v_p_hashes) {
        output.p_hashes.insert(ph);
    }
    return output;
}