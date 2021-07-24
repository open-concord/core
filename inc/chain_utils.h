#include <vector>

#include <boost/function.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool no_filter(json data);

bool type_filter(char qtype, json data);

json get_continuity_value(std::vector<json> defs,  std::string key);

std::vector<json> chain_search(std::vector<std::vector<std::string>> chain, char message_type, std::string target_trip, std::string key, boost::function<bool(json)> filter = no_filter, size_t start_b = -1, size_t end_b = -1);

std::string chain_encrypt(json data, std::string dsa_pri_key, std::string rsa_pub_key, std::string aes_key, char mt);