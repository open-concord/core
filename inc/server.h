#include "tree.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct user {
    std::string 
}

struct message {
    char type;
    json data;

}

class Server {
    private:
        Tree& tree;
        std::set<
}