#include <string>

#pragma once

std::string to_hexstr(unsigned char* chars, size_t size);

int from_hexstr(std::string data, unsigned char*& outchars, size_t* outsize);