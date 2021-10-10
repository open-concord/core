#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#include <array>

#include "../../inc/crypt++.h"

using namespace CryptoPP;

// skey = AES::DEFAULT_KEYLENGTH 
std::array<std::string, 2> AES_encrypt(std::string skey, std::string msg) {
    AutoSeededRandomPool prng;

    // convert binary key to SecByteBlock
    SecByteBlock key(reinterpret_cast<const byte*>(&skey[0]), skey.size());

    // intiallize nonce
    SecByteBlock nonce(AES_NONCELEN);
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
        // translate nonce to string
        std::string snonce(reinterpret_cast<const char*>(&nonce[0]), nonce.size());      
        // return cipher, nonce
        return {cipher, snonce};
    } catch (const /*crypto*/ Exception& err) {
        std::cerr << err.what() << "\n";
        exit(1);
    }
}

std::string AES_decrypt(std::string skey, std::string snonce, std::string cipher) {
    // tag size
    const int TAG_SIZE = 12;
    
    SecByteBlock key(reinterpret_cast<const byte*>(&skey[0]), skey.size());
    SecByteBlock nonce(reinterpret_cast<const byte*>(&snonce[0]), snonce.size());

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
    return std::string();
}

std::string AES_keygen() {
    AutoSeededRandomPool prng;
    SecByteBlock key(AES_KEYLEN);
    prng.GenerateBlock(key, key.size());
    std::string skey(reinterpret_cast<const char*>(&key[0]), key.size());
    return skey;
}