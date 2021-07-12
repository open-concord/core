#include <openssl/evp.h>
#include <string>
#include <array>

#pragma once

int sign(const char* msg, unsigned char** sig, size_t* slen, EVP_PKEY* prikey);

int verify(const char* msg, unsigned char** sig, size_t slen, EVP_PKEY* pubkey);

int dsaparamgen(EVP_PKEY* params);

int keygen_b(EVP_PKEY* params, EVP_PKEY* key);

int dsakeygen(EVP_PKEY* key);

int rsakeygen(EVP_PKEY* key);

int symm_encrypt(unsigned char* plaintext, size_t plaintextlen, unsigned char* key, unsigned char* iv, unsigned char* ciphertext, size_t* ciphertextlen);

int symm_decrypt(unsigned char* ciphertext, size_t ciphertextlen, unsigned char* key, unsigned char* iv, unsigned char* plaintext, size_t* plaintextlen);

int lockmessage(const char* plaintext, unsigned char*& ciphertext, size_t* ciphertextlen, EVP_PKEY* sigprikey, bool use_asymm, EVP_PKEY* encpubkey, unsigned char* serversecret);

int unlockmessage(unsigned char* ciphertext, size_t ciphertextlen, unsigned char*& plaintext, size_t* plaintextlen, unsigned char*& sig, size_t* siglen, bool use_asymm, EVP_PKEY* encprikey, unsigned char* serversecret);

int rsa_env_seal(EVP_PKEY** pub_key, unsigned char* plaintext, int plaintext_len,
	unsigned char** encrypted_key, int* encrypted_key_len, unsigned char* iv,
	unsigned char* ciphertext, int* ciphertext_len);

int rsa_env_open(EVP_PKEY *priv_key, unsigned char *ciphertext, int ciphertext_len,
	unsigned char* encrypted_key, int encrypted_key_len, unsigned char *iv,
	unsigned char* plaintext, int* plaintext_len);

std::string hex_lock(std::string data, std::string sigprikey, bool use_asymm, std::string encpubkey, std::string secret);

std::array<std::string, 2> hex_unlock(std::string data, bool use_asymm, std::string encprikey, std::string secret);

std::array<std::string, 2> hex_keygen(int (*keygenfunc)(EVP_PKEY*));

int hex_verify(std::string data, std::string sig, std::string sigpubkey);