#include "../inc/crypt++.h"

#include <iostream>
#include <vector>
#include <string>

using namespace CryptoPP;

int main() {
    std::string key = AES_keygen();
    std::string plain = "They should make Coffee a controlled substance";
    std::vector<std::string> aes = AES_encrypt(key, plain); // index 0 = cipher, index 1 = nonce
    std::cout << "AES KEY: " << key << "\n";
    std::cout << "DECRYPTED | PLAIN TEXT: " << plain << "\n";
    std::cout << "ENCRYPTED | CIPHER TEXT: " << aes.at(0) << "\n";
    std::cout << "ENCRYPTED | NONCE: " << aes.at(1) << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << AES_decrypt(key, aes.at(1), aes.at(0)) << "\n";


    std::cout << "RSA KEY GEN" << "\n";
    std::vector<std::string> rsakp = RSA_keygen();
    std::cout << "b64 PRIVATE RSA KEY | " << rsakp.at(0) << "\n";
    std::cout << "b64 PUBLIC RSA KEY | " << rsakp.at(1) << "\n";
    std::string rsa = RSA_encrypt(rsakp.at(1), plain);
    std::cout << "ENCRYPTED | CIPHER TEXT: " << rsa << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << RSA_decrypt(rsakp.at(1), rsa) << "\n";

    return 0;
}