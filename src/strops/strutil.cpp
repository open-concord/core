#include <cryptopp/osrng.h>

#include <string>

#include "../../inc/strops.hpp"

using namespace CryptoPP;

std::string gen_string(size_t len) {
    AutoSeededRandomPool prng;
    SecByteBlock buf(len);
    prng.GenerateBlock(buf, buf.size());
    std::string nstr(reinterpret_cast<const char*>(&buf[0]), buf.size());
    return nstr;
}
