#include <nlohmann/json.hpp>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <string>

#include "../../inc/tree.hpp"
#include "../../inc/strops.hpp"

std::string block::hash_concat() const {
    std::string concat_data = b64::encode(raw_time_to_string(this->time)) + this->s_trip + this->c_trip + this->cont; //b64 timestr encoding is only for safety
    for (auto ph : order_hashes(this->p_hashes)) concat_data += ph;
    return concat_data;
}

bool block::verify(int pow) const {
    std::string result_hash = hex::encode(gen::hash(false, this->hash_concat() + this->nonce));
    if (result_hash != this->hash) return false;
    for (int i = 0; i < pow; i++) {
        if (result_hash.at(i) != '0') return false;
    }
    return true;
}

json block::jdump() const {
    json output;
    std::string enc_time = b64::encode(raw_time_to_string(this->time));
    std::vector<std::string> v_p_hashes;
    for (const auto& ph : this->p_hashes) {
        v_p_hashes.push_back(ph);
    }
    output["d"] = std::vector<std::string>({enc_time, this->nonce, this->s_trip, this->c_trip, this->cont, this->hash});
    output["p"] = v_p_hashes;
    return output;
}

std::string block::dump() const {
  return this->jdump().dump();
}

block::block(
    std::string cont, 
    std::unordered_set<std::string> p_hashes, 
    int pow, 
    std::string s_trip, 
    unsigned long long set_time, 
    std::string c_trip
    ) {
    Miner local_miner(pow);
 
    this->time = set_time;
    this->s_trip = s_trip;
    this->c_trip = c_trip;
    this->cont = cont;
    this->p_hashes = p_hashes;
    std::array<std::string, 2> nonce_result = local_miner.generate_valid_nonce(false, this->hash_concat());
    this->nonce = nonce_result[0];
    this->hash = nonce_result[1]; 
}

block::block(json input) { 
    std::vector<std::string> data = input["d"].get<std::vector<std::string>>();
    this->time = string_to_raw_time(b64::decode(data[0]));
    this->nonce = data[1];
    this->s_trip = data[2];
    this->c_trip = data[3];
    this->cont = data[4];
    this->hash = data[5];
    std::vector<std::string> v_p_hashes = input["p"].get<std::vector<std::string>>();
    for (const auto& ph: v_p_hashes) {
        this->p_hashes.insert(ph);
    } 
}
