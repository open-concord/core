#include <string>
#include <fstream>

#pragma once

std::string calc_hash (bool use_disk, std::string target);

std::string gen_trip(size_t len = 16);