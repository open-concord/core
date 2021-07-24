#include <nlohmann/json.hpp>

#include "../../inc/crypt++.h"
#include "../../inc/b64.h"
#include "../../inc/chain_utils.h"

std::string chain_encrypt(json data, std::string dsa_pri_key, std::string rsa_pub_key, std::string aes_key, char mt) {
    std::string enc_string = data.dump();
    if (mt != 'p') { //no encryption for dec
        std::string rsa_pub_key;
        
        enc_string = b64_encode(lock_msg(enc_string, (mt == 'p'), b64_decode(dsa_pri_key), b64_decode(aes_key), b64_decode(rsa_pub_key)));
    }
    return enc_string;
}