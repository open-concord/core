#include <cryptopp/eccrypto.h>
#include <cryptopp/secblock.h>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>  // AutoSeededRandomPool
#include <cryptopp/oids.h> // secp256r1
#include <cryptopp/rijndael.h> // AES
#include <cryptopp/gcm.h>  // GCM
#include <cryptopp/hex.h> // HexEncoder/Decoder

#include <iostream>
#include <string>
#include <sstream>

using namespace CryptoPP;

struct dhms { // dhm suite
private:
  const int tag = 12; // GCM tag size
  ECDH<ECP>::Domain ecd; // ecdhm domain
  // keys
  SecByteBlock pri; // own private
  SecByteBlock pub; // own public
  SecByteBlock ppub; // peer public
  SecByteBlock shared; // both shared
  // key utility (interfaced via hex)
  SecByteBlock _Set(std::string k); // used to set keys
  std::string _Get(SecByteBlock* k); // used to get keys
public:
  // generators
  dhms();
  void Keys(); // generate key pair
  void Gen(); // this may take a bit if ur a potato
  // setters/getters
  void Peer(std::string p); // set ppub
  std::string Public(); // pull pub key
  std::string Shared(); // get shared secret
  std::string AE(std::string in); // Authenticated Encryption
  std::string AD(std::string in); // Authenticated Decryption
};
