#include <string>
#include <fstream>
#include <map>
#include <iostream>

#include "../../inc/rw.h"

RW::RW() {}

std::string RW::read(std::string path) {
    try {
        std::string temp;
        std::string out;
        std::ifstream f (path);
        if (f.is_open()) {
            while (getline (f, temp)){
                out+=temp;
            }
            f.close();
        } else {
            std::cout << "ERROR: Unable to open file";
        }
        return out;
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

void RW::write(std::string path, std::string content) {
    try {
        std::ofstream outf;
        outf.open(path);
        outf << content;
        outf.close();
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

std::string RW::b64(std::string path, bool encode, std::string content = "") {
    try {
        if (encode) {
            // base 64 encode
            return base64_encode(this->read(path));
        } else {
            // base 64 decode
            return base64_decode(content);
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
    }
}