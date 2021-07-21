#include "../inc/crypt++.h"

#include <iostream>
#include <string>
#include <array>
#include <cassert>

int main() {
    std::string plain = "hunting for dem bugs";
    
    std::cout << "AES KEY GEN" << "\n";
    std::string key = AES_keygen();
    std::cout << "AES KEY GEN SUCCESSFUL" << "\n";
    std::array<std::string, 2> aes = AES_encrypt(key, plain);
    std::cout << "DECRYPTED | PLAIN TEXT: " << plain << "\n";
    std::cout << "ENCRYPTED | CIPHER TEXT: " << aes[0] << "\n";
    std::cout << "ENCRYPTED | NONCE: " << aes[1] << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << AES_decrypt(key, aes.at(1), aes.at(0)) << "\n";
    //assert(plain == AES_decrypt(key, aes[1], aes[0]));


    std::cout << "RSA KEY GEN" << "\n";
    std::array<std::string, 2> rsakp = RSA_keygen();
    std::cout << "RSA KEY GEN SUCCESSFUL" << "\n";
    std::cout << "b64 PRIVATE RSA KEY | " << b64_encode(rsakp[0]) << "\n";
    std::cout << "b64 PUBLIC RSA KEY | " << b64_encode(rsakp[1]) << "\n";
    std::string rsa = RSA_encrypt(rsakp[1], plain);
    std::cout << "ENCRYPTED | CIPHER TEXT: " << b64_encode(rsa) << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << RSA_decrypt(rsakp[0], rsa) << "\n";
    //assert(plain == RSA_decrypt(rsakp[0], rsa));

    std::cout << "DSA KEY GEN" << "\n";
    std::array<std::string, 2> dsakp = DSA_keygen();
    std::cout << "DSA KEY GEN SUCSESSFUL" << "\n";
    std::cout << "b64 PRIVATE DSA KEY | " << b64_encode(dsakp[0]) << "\n";
    std::cout << "b64 PUBLIC DSA KEY | " << b64_encode(dsakp[1]) << "\n";
    std::string dsa = DSA_sign(dsakp[0], plain);
    std::cout << "ENCRYPTED | SIGNATURE: " << b64_encode(dsa) << "\n";
    std::cout << "DECRYPTED | VALID TEXT: " << DSA_verify(dsakp[1], dsa, plain) << "\n";
    //assert(DSA_verify(dsakp[1], dsa) == 1);
    return 0;
}

/*int main() {
    
    for (size_t i = 0; i < 128; i++) {
        maint();
        std::cout << "Finished on " << i << "\n";
    }
}*/