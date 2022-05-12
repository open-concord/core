#include "../../inc/crypt.hpp"
#include <cryptopp/dsa.h>
#include <cryptopp/osrng.h>

using namespace CryptoPP;

std::array<std::string, 2> cDSA::keygen() {
    AutoSeededRandomPool rng;
    // Private
    DSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, DSA_KEYLEN);

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

    // Return BER keys
    /** (maybe the order should be conventionalized?)
     * server.hpp does pub first, so maybe migrate to that later?
     */
    return {encodedPrivateKey, encodedPublicKey};
}

std::string cDSA::sign(std::string encodedPrivateKey, std::string msg) {
    AutoSeededRandomPool rng;
    // output
    std::string signature;

    // loading key
    DSA::PrivateKey privateKey;
    privateKey.Load(
        StringStore(
            encodedPrivateKey
        ).Ref()
    );

    // Initializing signer object
    DSA::Signer s(privateKey);

    StringSource (
        // input
        msg,
        // pump all (see RSA)
        true,
        // BufferedTransformation
        new SignerFilter(
            rng,
            s,
            new StringSink (signature)
        )
    );
    return signature;
}

bool cDSA::verify(std::string encodedPublicKey, std::string sig, std::string msg) {
    // return value
    bool legit; // phrased as a question, not an assertion

    // loading key
    DSA::PublicKey publicKey;
    publicKey.Load(
        StringStore(
            encodedPublicKey
        ).Ref()
    );
    // Initializing verifier object
    DSA::Verifier v(publicKey);
    // Checking
    StringSource(
        msg + sig,
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
