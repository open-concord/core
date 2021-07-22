#include "../inc/crypt++.h"
#include "../inc/b64.h"

#include <iostream>
#include <string>
#include <array>
#include <cassert>

int main() {
    std::string plain = "hunting for dem bugs";
    
    std::cout << "AES KEY GEN" << "\n";
    std::string key = AES_keygen();
    std::cout << "AES KEY: " << key.length() << "\n";
    std::cout << "AES KEY GEN SUCCESSFUL" << "\n";
    std::array<std::string, 2> aes = AES_encrypt(key, plain);
    std::cout << "DECRYPTED | PLAIN TEXT: " << plain << "\n";
    std::cout << "ENCRYPTED | CIPHER TEXT: " << aes[0].length() << "\n";
    std::cout << "ENCRYPTED | NONCE: " << aes[1].length() << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << AES_decrypt(key, aes.at(1), aes.at(0)) << "\n";
    //assert(plain == AES_decrypt(key, aes[1], aes[0]));


    std::cout << "RSA KEY GEN" << "\n";
    std::array<std::string, 2> rsakp = RSA_keygen();
    std::cout << "RSA KEY GEN SUCCESSFUL" << "\n";
    std::cout << "b64 PRIVATE RSA KEY | " << b64_encode(rsakp[0]) << "\n";
    std::cout << "b64 PUBLIC RSA KEY | " << b64_encode(rsakp[1]) << "\n";
    std::string rsa = RSA_encrypt(rsakp[1], std::string(410, 'f'));
    std::cout << "ENCRYPTED | CIPHER TEXT: " << rsa.length() << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << RSA_decrypt(rsakp[0], rsa) << "\n";
    //assert(plain == RSA_decrypt(rsakp[0], rsa));

    std::cout << "DSA KEY GEN" << "\n";
    std::array<std::string, 2> dsakp = DSA_keygen();
    std::cout << "DSA KEY GEN SUCSESSFUL" << "\n";
    std::cout << "b64 PRIVATE DSA KEY | " << b64_encode(dsakp[0]) << "\n";
    std::cout << "b64 PUBLIC DSA KEY | " << b64_encode(dsakp[1]) << "\n";
    std::string dsa = DSA_sign(dsakp[0], plain);
    std::cout << "ENCRYPTED | SIGNATURE: " << dsa.length() << "\n";
    std::cout << "DECRYPTED | VALID TEXT: " << DSA_verify(dsakp[1], dsa, plain) << "\n";
    //assert(DSA_verify(dsakp[1], dsa) == 1);
    
    std::string locked_msg = lock_msg(std::string(4170, 'f'), true, dsakp[0], "", rsakp[1]);
    std::cout << "LOCKED MSG | " << b64_encode(locked_msg) << "\n";
    std::cout << "LOCKED MSG LEN | " << locked_msg.length() << "\n";
    std::array<std::string, 2> unlocked_msg = unlock_msg(locked_msg, true, "", rsakp[0]);
    std::cout << "UNLOCKED MSG | " << unlocked_msg[0] << "\n";
    std::cout << "UNLOCKED MSG VALID TEXT | " << DSA_verify(dsakp[1], unlocked_msg[1], unlocked_msg[0]) << "\n";
    return 0;
}

/*int main() {
    
    for (size_t i = 0; i < 128; i++) {
        maint();
        std::cout << "Finished on " << i << "\n";
    }
}*/