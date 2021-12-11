#include "cryptopp/hex.h"
#include <string>

#include "../../inc/strenc.h"

using namespace CryptoPP;

std::string hex_encode(std::string in_string) {
    // output string
    std::string encoded;
    // copy input string to byte array
    SecByteBlock in_bytes(
        reinterpret_cast<const byte*>(&in_string[0]), in_string.size()
    );
    // pipe byte array through b64
    StringSource ss(in_bytes, in_bytes.size(), true,
        new HexEncoder(
            // pipe encoded byte array to output string
            new StringSink(encoded)
        )
    );
    // return output string
    std::string enc_string = encoded.substr(0, encoded.length());
    return enc_string;
}

std::string hex_decode(std::string encoded) {
    std::string decoded;

    StringSource ss(encoded, true,
        new HexDecoder(
            new StringSink(decoded)
        )
    );

    return decoded;
}
