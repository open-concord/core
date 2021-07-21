#include <string>
#include <iostream>
#include <array>

// AES
std::string AES_keygen();
std::array<std::string, 2> AES_encrypt(std::string hkey, std::string msg);
std::string AES_decrypt(std::string hkey, std::string hnonce, std::string hcipher);

// RSA
std::array<std::string, 2> RSA_keygen();
std::string RSA_encrypt(std::string encodedPublicKey, std::string msg);
std::string RSA_decrypt(std::string encodedPrivateKey, std::string cipher);

// DSA
std::array<std::string, 2> DSA_keygen();
std::string DSA_sign(std::string encodedPrivateKey, std::string message);
bool DSA_verify(std::string encodedPublicKey, std::string ciphertext);

// B64
std::string b64_encode(std::string in_string);
// can add a char array => b64 later for images etc
std::string b64_decode(std::string encoded);
