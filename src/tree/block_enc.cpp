#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include "tree.h"
#include "b64.h"

json block_to_json(block input) {
    json output;
    std::string enc_time = b64_encode(raw_time_to_string(input.time));
    output["d"] = std::vector<string>({enc_time, input.s_trip, input.c_trip, input.cont, input.hash});
    output["p"] = input.p_hashes;
    return output;
}

block json_to_block(json input) {
    block output;
    std::vector<string> data = input["d"];
    output.time = string_to_raw_time(b64_decode(data[0]));
    output.s_trip = data[1];
    output.c_trip = data[2];
    output.cont = data[3];
    output.hash = data[4];
    output.p_hashes = input["p"];
    return output;
}