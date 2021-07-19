#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#include "assert.h"

#include "../../inc/crypt++.h"

using namespace CryptoPP;

// skey = AES::DEFAULT_KEYLENGTH 
std::vector<std::string> AES_encrypt(std::string hkey, std::string msg) {
    AutoSeededRandomPool prng;

    // hex encoded to binary string
    std::string bkey = b64_decode(hkey);
    // convert binary key to SecByteBlock
    SecByteBlock key(reinterpret_cast<const byte*>(&bkey[0]), bkey.size());

    // intiallize nonce
    SecByteBlock nonce(AES::BLOCKSIZE);
    // populate nonce
    prng.GenerateBlock(nonce, nonce.size());
    // tag size
    const int TAG_SIZE = 12;
    // return value
    std::string cipher;

    // encrypt
    try {
        GCM<AES>::Encryption e;
        e.SetKeyWithIV(key, key.size(), nonce, nonce.size());

        StringSource (msg, true,
            new AuthenticatedEncryptionFilter(e,
                new StringSink(cipher), false, TAG_SIZE
            )
        );
        // convert string cipher to SecByteBlock (basically an array)
        SecByteBlock acipher(reinterpret_cast<const byte*>(&cipher[0]), cipher.size());
        // return cipher, nonce
        return {b64_encode(acipher), b64_encode(nonce)};
    } catch (const /*crypto*/ Exception& err) {
        std::cerr << err.what() << "\n";
        exit(1);
    }
}

std::string AES_decrypt(std::string hkey, std::string hnonce, std::string hcipher) {
    // tag size
    const int TAG_SIZE = 12;
    std::string bkey = b64_decode(hkey);
    std::string bnonce = b64_decode(hnonce);
    SecByteBlock key(reinterpret_cast<const byte*>(&bkey[0]), bkey.size());
    SecByteBlock nonce(reinterpret_cast<const byte*>(&bnonce[0]), bnonce.size());

    std::string cipher = b64_decode(hcipher);
    std::string recovered;
    try {
        GCM< AES >::Decryption d;
        d.SetKeyWithIV(key, key.size(), nonce, nonce.size());

        AuthenticatedDecryptionFilter df( d,
            new StringSink(recovered),
            AuthenticatedDecryptionFilter::DEFAULT_FLAGS, TAG_SIZE
        );

        // from the docs:
        // The StringSource dtor will be called immediately
        //  after construction below. This will cause the
        //  destruction of objects it owns. To stop the
        //  behavior so we can get the decoding result from
        //  the DecryptionFilter, we must use a redirector
        //  or manually Put(...) into the filter without
        //  using a StringSource.
        StringSource (cipher, true,
            new Redirector(df)
        );

        // If the object does not throw, here's the only
        //  opportunity to check the data's integrity
        if( true == df.GetLastResult() ) {
            return recovered;
        }
    } catch (Exception& err) {
        std::cerr << err.what() << "\n";
        exit(1);
    }
}

std::string AES_keygen() {
    AutoSeededRandomPool prng;
    SecByteBlock key(AES::BLOCKSIZE);
    prng.GenerateBlock(key, key.size());
    return b64_encode(key);
}