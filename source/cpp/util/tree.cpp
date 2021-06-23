/**
 * This file handles all tree-related logic for the client
 * 
 * NOTES:
 * Timestamping protocol follows the W3 standard;
 * "Complete date plus hours and minutes:
 *  YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)"
 */

#include <string>

#include "timewizard.cpp"
#include "hash.cpp"

// remove after debug
#include <iostream>

class Tree {
    private:

    public:
        // where h1 is the new(our) content, and h0 is prev hash
        std::string generate_branch(std::string h1, std::string h0 = "") {
            // h0 doesn't exist, so use duplicated h1
            if (h0.empty()) {h0 = h1;}
            // concatenation of h0+h1/h0
            std::string cat = h0+h1;

            // debugging
            std::cout << "Catted String: " << cat << "\n";

            // hashing w/ timestamp of h1
            std::string time = get_time();
            std::string hash = calc_hash(false, (cat+time));

            // if h0 = h1, hash(h0) == hash(h1)
            if (h0 == h1) {h0 = hash;}

            // time#content|hash$prevhash
            return time+"#"+h1+"|"+hash+"$"+h0;
        }

        // where h0 and h1 are child nodes, and h01 is block to be checked
        bool verify_integrity(std::string h0, std::string h1, std::string h01) {
            return false;
        }
};