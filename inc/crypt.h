#include <openssl/evp.h>
#include <string>
#include <array>

#pragma once

int sign(const char* msg, unsigned char** sig, size_t* slen, EVP_PKEY* prikey);

int verify(const char* msg, unsigned char** sig, size_t slen, EVP_PKEY* pubkey);

int paramgen(EVP_PKEY* params);

int keygen_b(EVP_PKEY* params, EVP_PKEY* key);

int keygen_a(EVP_PKEY* key);

int encrypt(unsigned char* plaintext, size_t plaintextlen, unsigned char* key, unsigned char* iv, unsigned char* ciphertext, size_t* ciphertextlen);

int decrypt(unsigned char* ciphertext, size_t ciphertextlen, unsigned char* key, unsigned char* iv, unsigned char* plaintext, size_t* plaintextlen);

int lockmessage(const char* plaintext, unsigned char* ciphertext, size_t* ciphertextlen, EVP_PKEY* userprikey, unsigned char* serversecret);

int unlockmessage(unsigned char* ciphertext, size_t ciphertextlen, unsigned char* plaintext, size_t* plaintextlen, EVP_PKEY* userpubkey, unsigned char* serversecret);

std::string extract_BIO_data(BIO* bio);

int set_BIO_data(BIO*& bio, std::string data_hex_str);

std::string hex_lock(std::string data, std::string secret, std::string prikey);

std::string hex_unlock(std::string data, std::string secret, std::string pubkey);

std::array<std::string, 2> hex_keygen();