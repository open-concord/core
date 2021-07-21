#include "../../inc/crypt++.h"
#include <string>

std::string undo_concat(std::string& target_str, size_t length) {
    std::string output = target_str.substr(target_str.length() - length);
    target_str = target_str.substr(0, target_str.length() - length);
    return output;
}

std::string lock_msg(std::string message, bool use_asymm, std::string dsa_pri_key, std::string aes_key, std::string rsa_pub_key) {
    std::string sig = DSA_sign(dsa_pri_key, message);
    std::string unified_plaintext = message + sig;
    std::string ciphertext;
    if (use_asymm) aes_key = AES_keygen();
    std::array<std::string, 2> encresults = AES_encrypt(aes_key, unified_plaintext);
    ciphertext += encresults[0];
    if (use_asymm) ciphertext += RSA_encrypt(rsa_pub_key, aes_key);
    ciphertext += encresults[1];
    return ciphertext;
}

std::array<std::string, 2> unlock_msg(std::string ciphertext, bool use_asymm, std::string aes_key, std::string rsa_pri_key) {
    std::string nonce = undo_concat(ciphertext, AES_NONCELEN);
    if (use_asymm) aes_key = RSA_decrypt(rsa_pri_key, undo_concat(ciphertext, RSA_KEYLEN / 8)); //get RSA keylen in chars (bytes)
    std::string plaintext = AES_decrypt(aes_key, nonce, ciphertext);
    std::string sig = undo_concat(plaintext, DSA_SIGLEN);
    return {plaintext, sig};
}