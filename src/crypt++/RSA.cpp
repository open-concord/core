#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#include <cryptopp/queue.h>
#include "assert.h"
#include <array>

#include <cryptopp/rsa.h>
#include <cryptopp/base64.h>

#include "../../inc/crypt++.h"
using namespace std;

// encode
std::string EncodePub(RSA::PublicKey key) {
    std::string out;
    ByteQueue queue;
    key.DEREncodePublicKey(queue);
    StringSink output(out);
    queue.CopyTo(output);
    output.MessageEnd();
    return b64_encode_string(out);
}
std::string EncodePri(RSA::PrivateKey key) {
    std::string out;
    ByteQueue queue;
    key.DEREncodePrivateKey(queue);
    StringSink output(out);
    queue.CopyTo(output);
    output.MessageEnd();
    return b64_encode_string(out);
}
// decode
void DecodePub(RSA::PublicKey key, std::string in) {
    std::string din = b64_decode(in);
    ByteQueue queue;
    StringSource output(din, true);
    output.TransferTo(queue);
    queue.MessageEnd();
    key.BERDecode(queue);
    key.Load(queue);
}
void DecodePri(RSA::PrivateKey key, std::string in) {
    std::string din = b64_decode(in);
    ByteQueue queue;
    StringSource output(din, true);
    output.TransferTo(queue);
    queue.MessageEnd();
    key.BERDecode(queue);
    key.Load(queue);
}

std::array<std::string, 2> RSA_keygen() {
    AutoSeededRandomPool rng;

    RSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, 4096);

    RSA::PublicKey publicKey(privateKey);

    return {EncodePri(privateKey), EncodePub(publicKey)};
}
std::string RSA_encrypt(std::string encKey, std::string msg) {
    AutoSeededRandomPool rng;
    // return value
    std::string cipher;
    // loading key
    RSA::PublicKey publicKey;
    DecodePub(publicKey, encKey);
    
    // encryptor object initialization
    RSAES_OAEP_SHA_Encryptor e(publicKey);

    StringSource sso(msg, true,
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

    StringSource sso(b64_decode(cipher), true,
        new PK_DecryptorFilter(rng, d,
            new StringSink(recovered)
        )
    );
    return recovered;
}