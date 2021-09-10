#include "tree.h"

#include <array>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct keypair {
    std::string DSA_key;
    std::string RSA_key;

    keypair(std::string dsak, std::string rsak) : DSA_key(dsak), RSA_key(rsak) {}
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

    user(std::string tripcode, keypair priset) : u_trip(tripcode), pri_keys(priset) {}
}

struct member {
    user* user_ref;
    std::vector<std::string> roles; //doesn't need to be ordered, is for convenience.
}

struct role {
    std::array<int, 6> features;
    unsigned int primacy;

    bool is_mute() return features[0];
    bool can_invite() return features[1];
    bool can_rem() return features[2];
    bool can_rgrant() return features[3];
    bool can_rcreate() return features[4];
    bool can_edit() return features[5];
}

struct message {
    std::string hash;
    char supertype;
    char type;
    json data;

    block* ref;
}

struct branch {
    std::string first_hash;
    std::vector<messages> messages;
    branch_context ctx;

    std::unordered_set<branch*> pt_c_branches;
    std::unordered_set<branch*> pt_p_branches;
}

std::string content_hash_concat(std::string time, std::string s_trip, std::unordered_set<std::string> p_hashes);

class branch_context {
    public:
        json settings;
        std::map<std::string, member> members;
        std::map<std::string, role> roles;

        branch_context();

        int min_primacy(member target);

        apply_data()
}

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
    
        member create_member(keypair pub_keys, std::vector<std::string> init_roles = std::vector<std::string>());

        bool apply_data(branch_context& ctx, json claf_data, std::string content, std::string signature, std::string content_hash)

        branch get_root_branch();

        void send_message(user author, json content);

        void add_block(std::string hash);
}
