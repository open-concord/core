#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
// openssl
#include <openssl/sha.h>
#include <openssl/rand.h>

class Miner {
    private:
        int pow;
        std::string hash;
    public:
        Miner(int POW_req) {
            this->pow = POW_req;
        };

        // read from file or raw      
        std::string calc_hash (bool use_disk, std::string target) {
            // if use_disk is true, target is a file name
            // if use_disk isn't than it's raw text

            unsigned char outhash[SHA256_DIGEST_LENGTH];
            std::string outstr;

            // init sha
            SHA256_CTX context;
            SHA256_Init(&context);

            if (use_disk) { // target is filename (disk)
                std::ifstream fl(target);
                fl.seekg(0, std::ios::end);
                size_t len = fl.tellg();
                char *ret = new char[len];
                fl.seekg(0, std::ios::beg);
                fl.read(ret, len);
                fl.close();
                outstr = ret;
            } else { // target is raw message (mem)
                outstr = target;
            }
            // finishing sha
            SHA256_Update(&context, outstr.c_str(), outstr.size());
            SHA256_Final(outhash, &context);
            std::stringstream strs;
            //bytearray to stringstream
            for (int i=0; i<SHA256_DIGEST_LENGTH; i++) {
                strs << std::hex << std::setw(2) << std::setfill('0') << (int)outhash[i];
            }
            return strs.str();
        };


        // checking given hash for compliance w/ chain pow
        bool check_valid_hash(std::string hash) {
            // check first <pow> chars for 0
            int pow_min = this->pow;
            for (int i=0;i<=pow_min; i++) {
                if (hash.at(i) != '0') {return false;}
            };
            return true;
        };

        // generating nonce
        std::string generate_nonce() {
            unsigned char nnonce[16];
            int rc = RAND_bytes(nnonce, sizeof(nnonce));
            if (rc == 1) {
                std::string snonce(reinterpret_cast<char*>(nnonce), sizeof(nnonce));
                return snonce;
            }
        };

        // genning hash
        std::string generate_valid_hash(std::string content) {
            std::string rhash = this->calc_hash(false, content); // working hash
            std::string nonce; // nonce temp val

            while (!this->check_valid_hash(rhash)) {
                nonce = generate_nonce();
                rhash = calc_hash(false, content+=nonce);
            }
            return nonce;
        };
};