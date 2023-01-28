/**
 * \addtogroup Core
 * \{
 */

#pragma once
#include <string>
#include <array>
#include <fstream>

// B64
namespace b64 {
  std::string encode(std::string in_string, int padded_len = -1);
  // can add a char array => b64 later for images etc
  std::string decode(std::string encoded);
}

// HEX
namespace hex {
  std::string encode(std::string in_string);
  std::string decode(std::string encoded);
}

// STR UTIL
namespace gen {
  std::string string(size_t len);
  // HASH
  std::string hash(bool use_disk, std::string target);
  std::string trip(std::string data, size_t outlen = 24);
}

// HASH GEN
class Miner {
  private:
    int pow;
    std::string hash;
  public:
    Miner(int POW_req);

    // checking given hash for compliance w/ chain pow
    bool check_valid_hash(std::string hash);

    // genning random nonce
    std::string generate_nonce();

    // genning nonce satisfying pow
    std::array<std::string, 2> generate_valid_nonce(bool debug_info, std::string content);
};

/**
 * \}
 */
