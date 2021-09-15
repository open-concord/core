#include "../../inc/tree.h"
#include "../../inc/hash.h"
#include "../../inc/strenc.h"
#include "../../inc/crypt++.h"
#include "../../inc/server.h"

#include <limits>
#include <map>
#include <unordered_set>
#include <vector>
#include <array>
#include <cmath>

#include <nlohmann/json.hpp>
#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

using json = nlohmann::json;

void branch_context::initialize_roles()  {
    //by default, the creator role exists.
    role creator_role;
    creator_role.features[0] = false;
    for (int i = 1; i < creator_role.features.size(); i++) creator_role.features[i] = 0;
    creator_role.primacy = 0;
    (this->roles)["creator"] = std::pair<role, int>(creator_role, 1);
}

branch_context::branch_context() {
    initialize_roles();
}

branch_context::branch_context(std::vector<branch_context> input_contexts) {
    for (auto input_context : input_contexts) {
        //rank selections for equal rank are random.
        //they can also be easily manipulated by repeated definition
        //this is of no consequence; the point is that action is chosen over inaction
        //all of these are privileges given to high-ranking users, and all of them can easily be removed if (to trivial and reversible effect) abused

        //make the highest-rank role selections and simultaneously get union of membership
        for (const auto& [hash, in_member] : input_context.members) {
            if ((this->members).count(hash) == 1) {
                for (const auto& [name, rank] : in_member.roles_ranks) {
                    if (std::abs(rank) > std::abs((this->members)[hash].roles_ranks[name])) {
                        (this->members)[hash].roles_ranks[name] = rank;
                    }
                }
            } 
            else {
                (this->members)[hash] = in_member;
            }
        }

        //also select role versions by rank
        for (const auto& [name, role_pair] : input_context.roles) {
            if (role_pair.second > (this->roles)[name].second) {
                (this->roles)[name] = role_pair;
            }
        }

        //very hacky, order- (which is random) dependent json merging.
        (this->settings).merge_patch(input_context.settings);

    }
}

unsigned int branch_context::min_primacy(member target) {
    unsigned int min = std::numeric_limits<int>::max();
    for (auto name_pair : target.roles_ranks) {
        if (name_pair.second < 0) continue;
        unsigned int role_primacy = ((this->roles)[name_pair.first]).first.primacy;
        if (role_primacy < min) {
            min = role_primacy;
        }
    }
    return min;
}

bool branch_context::has_feature(member target, int index) {
    bool result = false;
    for (auto name_pair : target.roles_ranks) {
        if (name_pair.second < 0) continue;
        bool role_feature = ((this->roles)[name_pair.first]).first.features[index];
        result = (result || role_feature);
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
    for (auto init_role : initial_roles) {
        temp_member.roles_ranks[init_role] = 1;
    }
    return temp_member;
}

void Server::load_branch_forward(std::string fb_hash) {
    block& active_block = tree.get_chain()[fb_hash]; //start on the branch's first block
    auto& target_pair = (this->branches)[fb_hash];
    std::unordered_set<std::string> seed_hashes;

    branch& target_branch = target_pair.first;

    //merge contexts
    branch_context ctx(target_pair.second);
    

    //see how far the linear part goes; we'll stop when there are multiple children
    while (true) {
        //message digestion logic
        try {
            std::array<std::string, 2> raw_unlocked = unlock_msg(active_block.cont, false, this->raw_AES_key);
            std::string content_hash = b64_encode(calc_hash(false, content_hash_concat(active_block.time, active_block.s_trip, active_block.p_hashes)));
            json claf_data = json::parse(raw_unlocked[0]);
            if (apply_data(ctx, claf_data, raw_unlocked[0], raw_unlocked[1], content_hash)) {
                //add an actual message if we can
                message result;
                result.hash = active_block.hash;
                result.supertype = std::string(claf_data["st"]).at(0);
                result.type = std::string(claf_data["t"]).at(0);
                result.data = claf_data["d"];
                result.ref = &active_block;
                target_branch.messages.push_back(result);
            }
            else {
                throw; //failure to apply data
            }
        }
        catch(int err) {
            //something should go here
        }
        //see if we're still linear
        std::unordered_set<std::string> intraserver_c_hashes;
        for (auto c_hash : active_block.c_hashes) {
            if (tree.get_chain()[c_hash].s_trip == (this->s_trip)) {
                intraserver_c_hashes.insert(c_hash);
            }
        }
        if (intraserver_c_hashes.size() == 1) {
            active_block = tree.get_chain()[*(intraserver_c_hashes.begin())];
        } else {
            //no longer linear
            seed_hashes = intraserver_c_hashes;
            break;
        }
    }
    target_branch.ctx = ctx; //context is established now.
    for (auto seed_hash : seed_hashes) {
        auto& seed_pair = (this->branches)[seed_hash];
        seed_pair.second.push_back(ctx);
        //if all the contexts are in, we can load the branch
        if (seed_pair.second.size() == tree.intraserver_parent_count(tree.get_chain()[seed_hash], this->s_trip)) {
            load_branch_forward(seed_hash);
        }
        (target_branch.pt_c_branches).insert(&seed_pair.first);
        seed_pair.first.pt_p_branches.insert(&target_branch);
    }
}
