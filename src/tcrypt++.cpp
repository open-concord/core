#include "../inc/crypt++.h"

#include <iostream>
#include <vector>
#include <string>

using namespace CryptoPP;

int main() {
    std::string key = AES_keygen();
    std::string plain = ";-; 2 ubuntu updates and 4 hours later; we have c++ AES typing";
    std::vector<std::string> encrypted = AES_encrypt(key, plain); // index 0 = cipher, index 1 = nonce
    std::cout << "AES KEY: " << key << "\n";
    std::cout << "DECRYPTED | PLAIN TEXT: " << plain << "\n";
    std::cout << "ENCRYPTED | CIPHER TEXT: " << encrypted.at(0) << "\n";
    std::cout << "ENCRYPTED | NONCE: " << encrypted.at(1) << "\n";
    std::cout << "DECRYPTED | RECOVERED TEXT: " << AES_decrypt(key, encrypted.at(1), encrypted.at(0)) << "\n";
    return 0;
}