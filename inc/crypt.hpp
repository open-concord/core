/**
 * \addtogroup Core
 * \{
 */

#pragma once
#include <string>
#include <iostream>
#include <array>
#include <cryptopp/aes.h>

#define AES_KEYLEN CryptoPP::AES::MAX_KEYLENGTH
#define AES_NONCELEN CryptoPP::AES::BLOCKSIZE
#define DSA_SIGLEN 64
#define DSA_KEYLEN 3072
#define RSA_KEYLEN 4096

// AES
namespace cAES {
  std::string keygen();
  std::array<std::string, 2> encrypt(std::string skey, std::string msg);
  std::string decrypt(
      std::string skey, 
      std::string snonce, 
      std::string cipher
  );
}
// RSA
namespace cRSA {
  std::array<std::string, 2> keygen();
  std::string encrypt(std::string encodedPublicKey, std::string msg);
  std::string decrypt(std::string encodedPrivateKey, std::string cipher);
}
// DSA
namespace cDSA {
  std::array<std::string, 2> keygen();
  std::string sign(std::string encodedPrivateKey, std::string msg);
  bool verify(
      std::string encodedPublicKey, 
      std::string sig, 
      std::string msg
  );
}

// Message Locking
namespace cMSG {
  std::string lock(
      std::string message, 
      bool use_asymm, 
      std::string dsa_pri_key, 
      std::string aes_key = "", 
      std::string rsa_pub_key = ""
  );
  std::array<std::string, 2> unlock(
      std::string ciphertext, 
      bool use_asymm, 
      std::string aes_key = "", 
      std::string rsa_pri_key = ""
  );
}
/**
 * \}
 */
