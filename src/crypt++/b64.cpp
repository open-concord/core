#include <cryptopp/base64.h>

#include <string>

#include "../../inc/crypt++.h"
using namespace CryptoPP;

std::string b64_encode(std::string in_string) {
    // output string
    std::string encoded;
    // copy input string to byte array
    SecByteBlock in_bytes(
        reinterpret_cast<const byte*>(&in_string[0]), in_string.size()
    );
    // pipe byte array through b64
    StringSource ss(in_bytes, in_bytes.size(), true,
        new Base64Encoder(
            // pipe encoded byte array to output string
            new StringSink(encoded)
        )
    );
    // return output string
    return encoded;
}

std::string b64_decode(std::string encoded) {
    std::string decoded;
   
    StringSource ss(encoded, true,
        new Base64Decoder(
            new StringSink(decoded)
        )
    );
    return decoded;
}