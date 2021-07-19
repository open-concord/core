#include <crypto++/cryptlib.h>
#include <crypto++/rijndael.h>
#include <crypto++/modes.h>
#include <crypto++/files.h>
#include <crypto++/osrng.h>
#include <crypto++/hex.h>
#include <crypto++/gcm.h>

using namespace CryptoPP;

// AES
std::string AES_keygen();
std::vector<std::string> AES_encrypt(std::string hkey, std::string msg);
std::string AES_decrypt(std::string hkey, std::string hnonce, std::string hcipher);

// RSA
std::vector<std::string> RSA_keygen();
std::string RSA_encrypt(std::string hpub, std::string msg);
std::string RSA_decrypt(std::string hpri, std::string hcipher);

// B64
std::string b64_encode(SecByteBlock ibytes);
std::string b64_decode(std::string encoded);

// SAVE
void save_keys_to_disk(const PublicKey& pubk, const PrivateKey& prik, std::string password);