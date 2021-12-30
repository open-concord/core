#include "cryptopp/sha.h"

#include <string>
#include <fstream>

#include "../../inc/strops.hpp"

using namespace CryptoPP;
// read from file or raw
std::string calc_hash (bool use_disk, std::string target) {
    // if use_disk is true, target is a file name
    // if use_disk isn't than it's raw text

    std::string outstr;

    if (use_disk) { // target is filename (disk)
        std::ifstream fl(target);
        fl.seekg(0, std::ios::end);
        size_t len = fl.tellg();
        char *ret = new char[len];
        fl.seekg(0, std::ios::beg);
        fl.read(ret, len);
        fl.close();
        outstr = ret;
    } else { // target is raw message (mem)
        outstr = target;
    }

    byte const* pbData = (byte*) outstr.data();
    unsigned int nDataLen = outstr.size();
    byte abDigest[SHA256::DIGESTSIZE];

    SHA256().CalculateDigest(abDigest, pbData, nDataLen);

    std::string output(reinterpret_cast<const char*>(&abDigest[0]), SHA256::DIGESTSIZE);
    return output;
}

std::string gen_trip(std::string data, size_t outlen) {
    std::string raw_hash = b64_encode(calc_hash(false, data), outlen);
    return raw_hash.substr(0, outlen);
}
