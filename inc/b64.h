#include <string>

#ifndef B64_H_
#define B64_H_

// B64
std::string b64_encode(std::string in_string);
// can add a char array => b64 later for images etc
std::string b64_decode(std::string encoded);

std::string gen_string(size_t len);

#endif