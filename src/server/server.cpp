#include "../../tree.h"
#include "../../hash.h"
#include "../../strenc.h"
#include "../../crypt++.h"

#include <limits>
#include <map>
#include <unordered_set>
#include <vector>
#include <array>
#include <nlohmann/json.hpp>
#include <boost/bind/bind.hpp>


using json = nlohmann::json;

branch_context::branch_context()  {
    //by default, the creator role exists.
    role creator_role;
    creator_role.is_muted = false;
    creator_role.can_invite = true;
    creator_role.can_rem = true;
    creator_role.can_role_grant = true;
    creator_role.can_role_create = true;
    creator_role.can_configure = true;
    creator_role.primacy = 0;
    roles["creator"] = creator_role;
}

unsigned int branch_context::min_primacy(member target) {
    unsigned int min = std::numeric_limits<int>::max();
    for (auto name : member.roles) {
        unsigned int role_primacy = ((this->roles)[name]).primacy;
        if (role_primacy < min) {
            min = role_primacy;
        }
    }
    return min;
}

bool branch_context::has_feature(member target, int index) {
    bool result = false;
    for (auto name : member.roles) {
        bool role_feature = ((this->roles)[name]).features[index];
        result = (result || role_feature)
    }
    return result;
}

Server::Server(Tree& parent_tree, std::string AES_key, user load_user) : tree(parent_tree) {
    this->raw_AES_key = b64_decode(AES_key);
    this->s_trip = gen_trip(AES_key, 24);
    std::unordered_set<std::string> root_hashes = this->tree.get_qualifying_hashes(boost::bind(&Tree::is_intraserver_orphan, _1, _2, this->s_trip));
    assert(root_hashes.size() == 1); //0 means the server doesn't exist. 2+ shouldn't be possible, as there are checks at every level for server connection.
    this->root_fb = *(root_hashes.begin());

    load_branch_forward(this->root_fb);
}

member Server::create_member(keypair pub_keys, std::vector<std::string> initial_roles) {
    user temp_user(pub_keys);
    (this->known_users)[temp_user.u_trip] = temp_user;
    member temp_member;
    temp_member.user_ref = &((this->known_users)[temp_user.u_trip]);
    temp_member.roles = initial_roles;
    return temp_member;
}

void Server::load_branch_forward(std::string fb_hash, branch_context ctx) {
    block& active_block = tree.get_chain()[fb_hash]; //start on the branch's first block
    branch& target_branch = (this->branches)[fb_hash];
    //see how far the linear part goes
    while (tree.intraserver_child_count(active_block) == 1) {
        //message digestion logic
        try {
            std::array<std::string, 2> raw_unlocked = unlock_msg(active_block.cont, false, this->raw_AES_key);
            std::string content_hash = b64_encode(calc_hash(false, content_hash_concat(active_block.time, active_block.s_trip, active_block.p_hashes)));
            json claf_data = json::parse(content);
            if (apply_data(ctx, claf_data, raw_unlocked[0], raw_unlocked[1], content_hash)) {
                //add an actual message if we can
                message result;
                result.hash = active_block.hash;
                result.supertype = std::string(claf_data["st"]);
                result.type = std::string(claf_data["t"]);
                result.data = claf_data["d"];
                result.ref = &active_block;
                target_branch.messages.push_back(result);
            }
            else {
                throw; //failure to apply data
            }
        }

        for (auto child_block : active_block.c_hashes) {
            if ()
        }
    }
    target_branch.ctx = ctx; //context is established now.

    
}
