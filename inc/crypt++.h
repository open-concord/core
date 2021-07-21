#include <string>
#include <iostream>
#include <array>
#include <cryptopp/aes.h>


#pragma once

#define AES_KEYLEN CryptoPP::AES::MAX_KEYLENGTH
#define AES_NONCELEN CryptoPP::AES::BLOCKSIZE
#define DSA_SIGLEN 64
#define DSA_KEYLEN 3072
#define RSA_KEYLEN 4096

// AES
std::string AES_keygen();
std::array<std::string, 2> AES_encrypt(std::string skey, std::string msg);
std::string AES_decrypt(std::string skey, std::string snonce, std::string cipher);

// RSA
std::array<std::string, 2> RSA_keygen();
std::string RSA_encrypt(std::string encodedPublicKey, std::string msg);
std::string RSA_decrypt(std::string encodedPrivateKey, std::string cipher);

// DSA
std::array<std::string, 2> DSA_keygen();
std::string DSA_sign(std::string encodedPrivateKey, std::string msg);
bool DSA_verify(std::string encodedPublicKey, std::string sig, std::string msg);

// B64
std::string b64_encode(std::string in_string);
// can add a char array => b64 later for images etc
std::string b64_decode(std::string encoded);

// Message Locking
std::string lock_msg(std::string message, bool use_asymm, std::string dsa_pri_key, std::string aes_key = "", std::string rsa_pub_key = "");
std::array<std::string, 2> unlock_msg(std::string ciphertext, bool use_asymm, std::string aes_key = "", std::string rsa_pri_key = "");
