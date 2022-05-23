#pragma once

#include <array>
#include <cmath>
#include <cassert>
#include <functional>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "../../../inc/proto.hpp"
#include "../../../inc/crypt.hpp"
#include "../../../inc/strops.hpp"
#include "../../../inc/tree.hpp"

#include "birank.hpp"
#include "bijson.hpp"
#include "strman.hpp"
#include "content.hpp"
#include "branch.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

bijson t;

class Server {
  private:
    user luser;
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
        std::string prev_AES_key = std::string(),
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
  void ConnHandle(ConnCtx* c) override;
};
