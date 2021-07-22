// B64
std::string b64_encode(std::string in_string);
// can add a char array => b64 later for images etc
std::string b64_decode(std::string encoded);

std::string gen_string(size_t quad_chars);