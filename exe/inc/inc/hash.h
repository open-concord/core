#include <string>
#include <fstream>

#pragma once

std::string calc_hash(bool use_disk, std::string target);

std::string gen_trip(std::string data, size_t outlen = 24);