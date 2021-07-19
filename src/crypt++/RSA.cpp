#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#include "assert.h"
#include <cryptopp/rsa.h>

#include "../../inc/crypt++.h"
using namespace std;

// encode
std::string EncodePub(RSA::PublicKey key) {
    std::string out;
    StringSink output(out);
    key.DEREncode(output);
    return b64_encode_string(out);
}
std::string EncodePri(RSA::PrivateKey key) {
    std::string out;
    StringSink output(out);
    key.DEREncode(output);
    return b64_encode_string(out);
}
// decode
void DecodePub(RSA::PublicKey key, std::string in) {
    b64_decode(in);
    StringSource output(in, true);
    key.BERDecodePublicKey(output, false, sizeof(output));
}
void DecodePri(RSA::PrivateKey key, std::string in) {
    b64_decode(in);
    StringSource output(in, true);
    key.BERDecodePrivateKey(output, false, sizeof(output));
}

std::vector<std::string> RSA_keygen() {
    AutoSeededRandomPool rng;

    InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 3072);

    RSA::PrivateKey privateKey(params);
    RSA::PublicKey publicKey(params);

    return {EncodePri(privateKey), EncodePub(publicKey)};
}
std::string RSA_encrypt(std::string encKey, std::string msg) {
    AutoSeededRandomPool rng;
    std::string cipher;

    // loading key
    RSA::PublicKey publicKey;
    DecodePub(publicKey, encKey);
    RSAES_OAEP_SHA_Encryptor e(publicKey);

    StringSource ss1(msg, true,
        new PK_EncryptorFilter(rng, e,
            new StringSink(cipher)
        )
    );
    return b64_encode_string(cipher);
}
std::string RSA_decrypt(std::string encKey, std::string cipher) {
    AutoSeededRandomPool rng;
    std::string recovered;

    // loading key
    RSA::PrivateKey privateKey;
    DecodePri(privateKey, encKey);

    RSAES_OAEP_SHA_Decryptor d(privateKey);

    StringSource ss2(b64_decode(cipher), true,
        new PK_DecryptorFilter(rng, d,
            new StringSink(recovered)
        )
    );
    return recovered;
}