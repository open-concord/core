#include <string>
#include <functional>
#include <map>
#include <fstream>

class RW {
    private:
        // file types mapped to parse functions
        std::map<std::string, std::string (*)(std::string)> tmap;
    public:
        // init function
        RW();
        // read file, return string
        std::string read(std::string path);
        // write to filepath with given string
        void write(std::string path, std::string content);
        // b64 encode/decode
        std::string b64(std::string path, bool encode, std::string content);
};

//
//  base64 encoding and decoding with C++.
//  Version: 2.rc.08 (release candidate)
//

#ifndef BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A
#define BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A

#include <string>

#if __cplusplus >= 201703L
#include <string_view>
#endif  // __cplusplus >= 201703L

std::string base64_encode     (std::string const& s, bool url = false);
std::string base64_encode_pem (std::string const& s);
std::string base64_encode_mime(std::string const& s);

std::string base64_decode(std::string const& s, bool remove_linebreaks = false);
std::string base64_encode(unsigned char const*, size_t len, bool url = false);

#if __cplusplus >= 201703L
//
// Interface with std::string_view rather than const std::string&
// Requires C++17
// Provided by Yannic Bonenberger (https://github.com/Yannic)
//
std::string base64_encode     (std::string_view s, bool url = false);
std::string base64_encode_pem (std::string_view s);
std::string base64_encode_mime(std::string_view s);

std::string base64_decode(std::string_view s, bool remove_linebreaks = false);
#endif  // __cplusplus >= 201703L

#endif /* BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A */