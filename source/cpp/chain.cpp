// blockchain management + block propagation
// this is where the isolation of each ledger comes into effect
#include <vector>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "crypt.cpp"

// block gen
class block {
    private:
        std::string content;
        std::string hash;
        std::string prevhash;
        std::string timestamp;
        int pow = 3;
    public:
        block(std::string content, std::string _prevhash) {
            this->prevhash = _prevhash;
            this->content = content;
            this->hash = calc_hash(false, this->content); // do sha256 hash
            this->timestamp = get_timestamp();
        };
        std::string get_timestamp() { // placeholder for right now, will change based on futher needs
            char outstr[200];
            time_t t;
            struct tm *tmp;
            const char* fmt = "%a, %d %b %y %T %z";

            t = time(NULL);
            tmp = gmtime(&t);
            if (tmp == NULL) {
                perror("gmtime error");
                exit(EXIT_FAILURE);
            }

            if (strftime(outstr, sizeof(outstr), fmt, tmp) == 0) { 
                fprintf(stderr, "strftime returned 0");
                exit(EXIT_FAILURE); 
            } 
            printf("%s\n", outstr);
            return outstr;
        };

        // checking given hash for compliance w/ chain pow
        bool check_valid_hash(std::string hash) {
            // check first <pow> chars for 0
            for (unsigned i=0;i<hash.length(); ++i) {
                if (i<=this->pow) {
                    if (hash.at(i) != 0) {return false;}
                } else {return true;}
            };
        };

        // genning hash
        void generate_valid_hash() {
            std::string rhash;
            int iter = 0;
            while (!this->check_valid_hash(rhash)) {
                // timestamping (NOT DONE)
                std::string temp = this->format()+std::to_string(iter);
                rhash = calc_hash(false, temp);
                iter += 1;
            }
            this->hash = rhash;
        };
        // text formatting
        std::string format() {
            //data#timestamp#prevhash
            std::cout << this->content+="#"+=this->timestamp+="#"+=this-prevhash;
            return this->content+="#"+=this->timestamp+="#"+=this-prevhash;
        };
};