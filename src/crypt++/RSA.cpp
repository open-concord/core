#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/gcm.h>
#include <array>
#include <cryptopp/rsa.h>

#include "../../inc/crypt++.h"
using namespace std;
using namespace CryptoPP;

std::array<std::string, 2> RSA_keygen() {
    AutoSeededRandomPool rng;

    RSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, 4096);

    RSA::PublicKey publicKey;
    publicKey.AssignFrom(privateKey);

    // Validating
    if (!privateKey.Validate(rng, 3) || !publicKey.Validate(rng, 3)) {
        throw "RSA KeyGen produced invalid keys";
    }
    // No issues
    // Save keys to strings (encoded as per BER)
    std::string encodedPrivateKey, encodedPublicKey;
    privateKey.Save(
        StringSink(encodedPrivateKey).Ref()
    );
    publicKey.Save(
        StringSink(encodedPublicKey).Ref()
    );

    // return B64 encoded versions of BER encoded keys    
    return {b64_encode(encodedPrivateKey), b64_encode(encodedPublicKey)};
}

std::string RSA_encrypt(std::string encodedPublicKey, std::string msg) {
    AutoSeededRandomPool rng;
    // return value
    std::string cipher;

    // loading key
    RSA::PublicKey publicKey;
    publicKey.Load(
        StringStore(
            b64_decode(encodedPublicKey)
        ).Ref()
    );
    
    // encryptor object initialization
    RSAES_OAEP_SHA_Encryptor e(publicKey);

    StringSource (
        // msg input
        msg,
        // pump all (pass input to BufferedTransform)
        true,
        // BufferedTransform
        new PK_EncryptorFilter(rng, e,
            new StringSink(cipher)
        )
    );
    // return B64 encoded message
    return b64_encode(cipher);
}

std::string RSA_decrypt(std::string encodedPrivateKey, std::string cipher) {
    AutoSeededRandomPool rng;
    // return value
    std::string recovered;

    // loading key
    RSA::PrivateKey privateKey;
    privateKey.Load(
        StringStore(
            b64_decode(encodedPrivateKey)
        ).Ref()
    );

    // Intialize Decryptor object
    RSAES_OAEP_SHA_Decryptor d(privateKey);

    StringSource (
        // msg input
        b64_decode(cipher),
        // pump all (pass input to BufferedTransform)
        true,
        // BufferedTransform
        new PK_DecryptorFilter(rng, d,
            new StringSink(recovered)
        )
    );
    // return raw message
    return recovered;
}