#include <string>

// hashing
#include "hash.cpp"

class Miner {
    private:
        int pow;
        std::string hash;
    public:
        Miner(int POW_req) {
            this->pow = POW_req;
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
            std::string rhash = calc_hash(false, content); // working hash
            std::string nonce; // nonce temp val

            while (!this->check_valid_hash(rhash)) {
                nonce = generate_nonce();
                rhash = calc_hash(false, content+=nonce);
            }
            return nonce;
        };
};