#include "tree.h"
#include "hash.h"
#include "crypt++.h"
#include "strenc.h"

#include <array>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct keypair {
    std::string DSA_key;
    std::string RSA_key;

    keypair() {}

    keypair(std::string dsak, std::string rsak) : DSA_key(dsak), RSA_key(rsak) {}
};

struct user {
    std::string u_trip;
    keypair pub_keys;
    keypair pri_keys;
    //bool empty = false;

    user() {
        std::array<std::string, 2> DSA_keys = DSA_keygen();
        std::array<std::string, 2> RSA_keys = RSA_keygen();
        pub_keys = keypair(b64_encode(DSA_keys[1]), b64_encode(RSA_keys[1]));
        pri_keys = keypair(b64_encode(DSA_keys[0]), b64_encode(RSA_keys[0]));
        this->u_trip = gen_trip(pub_keys.DSA_key + pub_keys.RSA_key, 24);
    }

    user(keypair pubset) : pub_keys(pubset) {
        this->u_trip = gen_trip(pubset.DSA_key + pubset.RSA_key, 24);
    }

    user(keypair pubset, keypair priset) : pub_keys(pubset), pri_keys(priset) {
        this->u_trip = gen_trip(pubset.DSA_key + pubset.RSA_key, 24);
    }

    user(std::string tripcode, keypair priset) : u_trip(tripcode), pri_keys(priset) {}
};

//rank that can express positive or negative values needs special properties
struct birank {
        int irank = 0;
        void orient_dir(bool dir);
        void increment();
        bool get_dir();
        operator bool();
};

bool operator== (birank b1, birank b2);
bool operator> (birank b1, birank b2);
bool operator< (birank b1, birank b2);

enum bijson_type {TERMINAL, MAP, VECT};

struct bijson {
    bijson_type type = TERMINAL;
    std::map<std::string, std::pair<bijson, birank>> map_values;
    std::vector<std::pair<bijson, birank>> vect_values;
    json base_json;

    bijson();
    bijson(json base);

    void set_key(std::string key, bijson value);
    void clear_key(std::string key);

    void set_type(bijson_type new_type);
    void set_base(json base);
    void set_keys(std::map<std::string, bijson> key_values);
    void clear_keys(std::vector<std::string> keys);
    //void set_indices(std::map<size_t, bijson> indice_values);
    //void clear_indices(std::vector<size_t> indices);
};

bijson merge(bijson bs1, bijson bs2);

struct member {
    std::string user_trip;
    std::map<std::string, birank> roles_ranks;
};

struct role {
    std::array<birank, 6> features;
    std::array<unsigned int, 2> primacy_rank;
    bool primacy() { return primacy_rank[0]; }
    bool is_mute() { return features[0]; }
    bool can_invite() { return features[1]; }
    bool can_rem() { return features[2]; }
    bool can_rgrant() { return features[3]; }
    bool can_rcreate() { return features[4]; }
    bool can_edit() { return features[5]; }
};

struct message {
    std::string hash;
    char supertype;
    std::string type;
    json data;
    json extra;
};

class branch_context {
    private:
        void initialize_roles();
    public:
        bijson settings;
        std::map<std::string, member> members;
        std::map<std::string, role> roles;

        branch_context();

        branch_context(std::vector<branch_context> input_contexts);

        unsigned int min_primacy(member target);

        bool has_feature(member target, int index);
};

struct branch {
    std::string first_hash;
    std::vector<message> messages;
    branch_context ctx;

    std::unordered_set<std::string> c_branch_fbs;
    std::unordered_set<std::string> p_branch_fbs;
};

std::string content_hash_concat(long long unsigned int time, std::string s_trip, std::unordered_set<std::string> p_hashes);
int encode_features(std::array<bool, 6>);
std::array<bool, 6> decode_features(int);

class Server {
    private:
        user luser;
        std::unordered_set<std::string> constraint_heads;
        std::unordered_set<std::string> constraint_path_fbs;
        std::unordered_set<std::string> constraint_path_lbs; //makes fbs search faster
        std::string s_trip;
        std::string raw_AES_key;
        std::string root_fb;
        std::map<std::string, user> known_users;
        std::map<std::string, branch> branches;

        void load_branch_forward(std::string fb_hash);

        void backscan_constraint_path(std::string lb_hash);
    public:
        Tree& tree;
        
        Server(Tree& parent_tree, std::string AES_key, user load_user = user(), std::string prev_AES_key = std::string(), std::unordered_set<std::string> heads = std::unordered_set<std::string>());
    
        member create_member(keypair pub_keys, std::vector<std::string> init_roles = std::vector<std::string>());

        bool apply_data(branch_context& ctx, json& extra, json claf_data, std::string content, std::string signature, std::string content_hash);

        branch get_root_branch();

        branch get_branch(std::string fb);

        std::string send_message(user author, json content, char st, std::string t = std::string(), std::unordered_set<std::string> p_hashes = std::unordered_set<std::string>());

        void add_block(std::string hash);

        void batch_add_blocks(std::unordered_set<std::string> hashes);
};