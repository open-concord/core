#include "tree.h"

#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct keypair {
    std::string DSA_key;
    std::string RSA_key;

    keypair(std::string dsak, std::string rsak) : DSA_key(dsak), RSA_key(rsak) {

    }
}

struct user {
    keypair pri_keys;
    keypair pub_keys;
    std::string u_trip;
    bool empty = false;

    user() {
        this->empty = true;
    }

    user(keypair pubset) : pub_keys(pubset) {
        this->trip = gen_trip(pubset.DSA_key + pubset.RSA_key, 24);
    }

    user(keypair pubset, keypair priset) : pub_keys(pubset), pri_keys(priset) {
        this->trip = gen_trip(pubset.DSA_key + pubset.RSA_key, 24);
    }

    user(std::string tripcode, keypair priset) : u_trip(tripcode), pri_keys(priset) {

    }
}

struct member {
    user* user_ref;
    std::vector<std::string> roles;
}

struct role {
    bool is_muted;
    bool can_invite;
    bool can_rem;
    bool can_role_grant;
    bool can_role_create;
    bool can_configure;
    unsigned int primacy;
}

struct message {
    std::string hash;
    char type;
    json data;

    block* ref;
}

struct branch {
    std::string first_hash;
    std::vector<messages> messages;

    std::unordered_set<branch*> pt_c_branches;
    std::unordered_set<branch*> pt_p_branches;
}

struct branch_context {
    json settings;
    std::map<std::string, member> members;
    std::map<std::string, role> roles;

    branch_context() {

    }
}

std::string content_hash_concat(std::string time, std::string s_trip, std::unordered_set<std::string> p_hashes);

class Server {
    private:
        Tree& tree;
        std::string s_trip;
        std::string raw_AES_key;
        std::string root_fb;
        std::map<std::string, user> known_users;
        std::map<std::string, role> known_roles;
        std::map<std::string, branch> branches;

        void load_branch_forward(std::string fb_hash, branch_context ctx = branch_context());
    public:
        Server(Tree& parent_tree, std::string AES_key, user load_user = user());

        branch get_root_branch();

        void send_message(user author, json content);

        void add_block(std::string hash);
}