#pragma once

#include <array>
#include <cmath>
#include <cassert>
#include <functional>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <proto.hpp>
#include <crypt.hpp>
#include <strops.hpp>
#include <tree.hpp>
#include <mutex>

#include "birank.hpp"
#include "bijson.hpp"
#include "strman.hpp"
#include "content.hpp"
#include "branch.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct keypair {
    std::string DSA;
    std::string RSA;

    keypair() {}
    keypair(std::string dsak, std::string rsak) : DSA(dsak), RSA(rsak) {}
};

struct user {
    std::string trip;
    keypair pubkeys, prikeys;
    // bool empty = false;

    user() {
        std::array<std::string, 2> DSA = cDSA::keygen();
        std::array<std::string, 2> RSA = cRSA::keygen();

        pubkeys = keypair(b64::encode(DSA[1]), b64::encode(RSA[1]));
        prikeys = keypair(b64::encode(DSA[0]), b64::encode(RSA[0]));

        this->trip = gen::trip(pubkeys.DSA + pubkeys.RSA, 24);
    }

    user(keypair pubset) : pubkeys(pubset) {
        this->trip = gen::trip(pubset.DSA + pubset.RSA, 24);
    }

    user(keypair pubset, keypair priset) : pubkeys(pubset), prikeys(priset) {
        this->trip = gen::trip(pubset.DSA + pubset.RSA, 24);
    }

    user(std::string trip, keypair priset) : trip(trip), prikeys(priset) {}
};

class Server {
  private:
    bool empty = true;
    user luser;
    std::mutex add_lock;

    std::unordered_set<std::string> constraint_heads, constraint_path_fbs, constraint_path_lbs /** makes fbs search faster */;

    std::string s_trip, raw_AES_key, root_fb;
    std::map<std::string, user> known_users;
    std::map<std::string, branch> branches;

    void load_branch_forward(std::string fb_hash);
    void backscan_constraint_path(std::string lb_hash);

  public:
    Tree& tree;
    
    Server(
        Tree& parent_tree,
        std::string AES_key,
        user load_user = user(),
        std::unordered_set<std::string> heads = std::unordered_set<std::string>()
    );
    
    member create_member(
        keypair pkeys,
        std::vector<std::string> init_roles = std::vector<std::string>()
    );
    
    bool apply_data(
        branch_context& ctx,
        json& extra,
        json claf_data,
        std::string content,
        std::string signature,
        std::string content_hash
    );

    void create_root(std::string prev_AES_key = std::string());

    std::string send_message(
        user author,
        json content,
        char st,
        std::string t = std::string(),
        std::unordered_set<std::string> p_hashes = std::unordered_set<std::string>()
    );

    /** get */
    branch get_root_branch();
    branch get_branch(std::string fb);
    
    /** add */
    void add_block(std::string hash);
    void add_batch(std::unordered_set<std::string> hashes);
};

struct CLAF : public Protocol {
private:

public:
  void ConnHandle(Conn* c) override;
  void NodeHandle(Node* n) override; // same as hclc
};
