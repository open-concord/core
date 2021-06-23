/**
 * This file handles all tree-related logic for the client
 * 
 * NOTES:
 * Timestamping protocol follows the W3 standard;
 * "Complete date plus hours and minutes:
 *  YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)"
 */

#include <string>
#include <optional>

#include "timewizard.cpp"
#include "hash.cpp"

// remove after debug
#include <iostream>

class tree {
    private:

    public:
        // where h1 is the new(our) content, and h0 is prev hash
        std::string generate_branch(std::optional<std::string> h0, std::string h1) {
            // h0 doesn't exist, so use duplicated h1
            if (!h0.has_value()) {h0.value() = h1;}
            // concatenation of h0+h1/h0
            std::string cat = h0.value()+h1;

            // debugging
            std::cout << "Catted String: " << cat << "\n";

            // hashing w/ timestamp of h1
            std::string time = get_time();
            std::string hash = calc_hash(false, (cat+time));

            // time#content|hash$prevhash
            return time+"#"+h1+"|"+hash+"$"+h0;
        }

        // gonna leave this for 1u to figure out
        bool verify_integrity() {
            return;
        }
};