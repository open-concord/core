#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include "tree.h"
#include "strenc.h"

json block_to_json(block input) {
    json output;
    std::string enc_time = b64_encode(raw_time_to_string(input.time));
    output["d"] = std::vector<std::string>({enc_time, input.nonce, input.s_trip, input.c_trip, input.cont, input.hash});
    output["p"] = input.p_hashes;
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
    output.p_hashes = input["p"].get<std::vector<std::string>>();
    return output;
}