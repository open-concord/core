#include "../inc/crypt++.h"

#include <iostream>
#include <string>
#include <array>

int main() {
    std::string plain = "hunting for dem bugs";
    
    std::cout << "AES KEY GEN" << "\n";
    std::string key = AES_keygen();
    std::cout << "AES KEY GEN SUCSESSFUL" << "\n";
    std::array<std::string, 2> aes = AES_encrypt(key, plain);
    std::cout << "DECRYPTED | PLAIN TEXT: " << plain << "\n";
    std::cout << "ENCRYPTED | CIPHER TEXT: " << aes[0] << "\n";
    std::cout << "ENCRYPTED | NONCE: " << aes[1] << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << AES_decrypt(key, aes.at(1), aes.at(0)) << "\n";


    std::cout << "RSA KEY GEN" << "\n";
    std::array<std::string, 2> rsakp = RSA_keygen();
    std::cout << "RSA KEY GEN SUCSESSFUL" << "\n";
    std::cout << "b64 PRIVATE RSA KEY | " << rsakp[0] << "\n";
    std::cout << "b64 PUBLIC RSA KEY | " << rsakp[1] << "\n";
    std::string rsa = RSA_encrypt(rsakp[1], plain);
    std::cout << "ENCRYPTED | CIPHER TEXT: " << rsa << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << RSA_decrypt(rsakp[0], rsa) << "\n";

    std::cout << "DSA KEY GEN" << "\n";
    std::array<std::string, 2> dsakp = DSA_keygen();
    std::cout << "DSA KEY GEN SUCSESSFUL" << "\n";
    std::cout << "b64 PRIVATE DSA KEY | " << dsakp[0] << "\n";
    std::cout << "b64 PUBLIC DSA KEY | " << dsakp[1] << "\n";
    std::string dsa = DSA_sign(dsakp[0], plain);
    std::cout << "ENCRYPTED | CONCATENATED TEXT: " << dsa << "\n";
    std::cout << "DECRYPTED | VALID TEXT: " << DSA_verify(dsakp[1], dsa) << "\n";
    return 0;
}