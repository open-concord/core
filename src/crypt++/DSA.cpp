#include "../../inc/crypt++.h"
#include <cryptopp/dsa.h>
#include <cryptopp/osrng.h>

using namespace CryptoPP;

std::array<std::string, 2> DSA_keygen() {
    AutoSeededRandomPool rng;
    // Private
    DSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, 3072);

    // Public
    DSA::PublicKey publicKey;
    publicKey.AssignFrom(privateKey);

    // Validating
    if (!privateKey.Validate(rng, 3) || !publicKey.Validate(rng, 3)) {
        throw "DSA KeyGen produced invalid keys";
    }

    // No issues
    std::string encodedPublicKey, encodedPrivateKey;

    // Save keys to strings (encoded as per BER)
    publicKey.Save(StringSink(encodedPublicKey).Ref());
    privateKey.Save(StringSink(encodedPrivateKey).Ref());

    // Return (B64 => BER) Encoded Keys
    return {b64_encode(encodedPrivateKey), b64_encode(encodedPublicKey)};
}

std::string DSA_sign(std::string encodedPrivateKey, std::string message) {
    AutoSeededRandomPool rng;
    // output
    std::string signature;

    // loading key
    DSA::PrivateKey privateKey;
    privateKey.Load(
        StringStore(
            b64_decode(encodedPrivateKey)
        ).Ref()
    );

    // Initializing signer object
    DSA::Signer s(privateKey);

    StringSource (
        // input
        message,
        // pump all (see RSA)
        true,
        // BufferedTransformation
        new SignerFilter(
            rng,
            s,
            new StringSink (signature)
        )
    );
    // return b64 encoded signature 
    return b64_encode(message+signature);
}

bool DSA_verify(std::string encodedPublicKey, std::string ciphertext) {
    // return value
    bool legit; // phrased as a question, not an assertion

    // loading key
    DSA::PublicKey publicKey;
    publicKey.Load(
        StringStore(
            b64_decode(encodedPublicKey)
        ).Ref()
    );
    // Initializing verifier object
    DSA::Verifier v(publicKey);
    // Checking
    StringSource(
        b64_decode(ciphertext),
        true,
        new SignatureVerificationFilter {
            v,
            new ArraySink (
                (byte*)&legit, sizeof(legit) /* should be 1 byte, but better safe than sorry */
            ),
            // PUT_RESULT flag
            // pipes SignatureVerificationFilter's output to the bool
            8
        }
    );

    return legit;
}