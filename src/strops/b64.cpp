#include <cryptopp/base64.h>

#include <string>

#include "../../inc/strops.hpp"

using namespace CryptoPP;

std::string b64::encode(std::string in_string, int padded_len) {
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
    std::string enc_string = encoded.substr(0, encoded.length() - 1); //a null character we don't want gets included
    if (padded_len != -1 && (size_t) padded_len > enc_string.length()) {
        enc_string += std::string(padded_len - enc_string.length(), '=');
    }
    return enc_string;
}

std::string b64::decode(std::string encoded) {
    std::string decoded;

    StringSource ss(encoded, true,
        new Base64Decoder(
            new StringSink(decoded)
        )
    );
    return decoded;
}
