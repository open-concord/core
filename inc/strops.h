#include <string>
#include <fstream>

#pragma once

// B64
std::string b64_encode(std::string in_string, int padded_len = -1);
// can add a char array => b64 later for images etc
std::string b64_decode(std::string encoded);

// HEX
std::string hex_encode(std::string in_string);
std::string hex_decode(std::string encoded);

std::string gen_string(size_t len);

// HASH
std::string calc_hash(bool use_disk, std::string target);

std::string gen_trip(std::string data, size_t outlen = 24);
