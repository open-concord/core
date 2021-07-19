#include <crypto++/cryptlib.h>
#include <crypto++/rijndael.h>
#include <crypto++/modes.h>
#include <crypto++/files.h>
#include <crypto++/osrng.h>
#include <crypto++/hex.h>
#include <crypto++/gcm.h>

using namespace CryptoPP;

std::vector<std::string> AES_encrypt(std::string hkey, std::string msg);
std::string AES_decrypt(std::string hkey, std::string hnonce, std::string hcipher);
std::string AES_keygen();

std::string hencode(SecByteBlock ibytes);
std::string hdecode(std::string istr);