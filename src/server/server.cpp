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
#include <functional>

#include <nlohmann/json.hpp>

using namespace std::placeholders;
using json = nlohmann::json;

Server::Server(Tree& parent_tree, std::string AES_key, user load_user, std::string prev_AES_key, std::unordered_set<std::string> heads) : tree(parent_tree), luser(load_user), constraint_heads(heads) {
    this->raw_AES_key = b64_decode(AES_key);
    this->s_trip = gen_trip(AES_key, 24);
    std::unordered_set<std::string> root_hashes = this->tree.get_qualifying_hashes(&Tree::is_intraserver_orphan);
    assert(root_hashes.size() <= 1); //0 means the server doesn't exist, but still, they could create it. 2+ shouldn't be possible, as there are checks at every level for server connection.
    if (root_hashes.size() == 1) {
        this->root_fb = *(root_hashes.begin());
        for (auto constraint_head : (this->constraint_heads)) {
            //a little hacky, but with no constraint heads there will be no scanning
            //strictly there should be an if statement on size != 0, but it doesn't change the logic.
            backscan_constraint_path(constraint_head);
        }
        load_branch_forward(this->root_fb);
    } else if (root_hashes.size() == 0) {
        json nserv_data = {
            {"cms", {
                {"enc_pubk", load_user.pub_keys.RSA_key},
                {"sig_pubk", load_user.pub_keys.DSA_key}
            }}
        };
        if (!prev_AES_key.empty()) nserv_data["prev_key"] = prev_AES_key;
        this->root_fb = send_message(load_user, nserv_data, 'a', "nserv");
    }
    tree.add_block_funcs[(this->s_trip)] = std::bind(&Server::add_block, this, _1);
    tree.batch_add_funcs[(this->s_trip)] = std::bind(&Server::batch_add_blocks, this, _1);
}

branch Server::get_root_branch() {
    return (this->branches)[this->root_fb];
}

branch Server::get_branch(std::string fb) {
    return (this->branches)[fb];
}

member Server::create_member(keypair pub_keys, std::vector<std::string> initial_roles) {
    user temp_user(pub_keys);
    (this->known_users)[temp_user.u_trip] = temp_user;
    member temp_member;
    temp_member.user_trip = temp_user.u_trip;
    for (auto init_role : initial_roles) {
        temp_member.roles_ranks[init_role].orient_dir(true);
    }
    return temp_member;
}

void Server::backscan_constraint_path(std::string lb_hash) {
    std::string working_hash = lb_hash;
    (this->constraint_path_lbs).insert(lb_hash);

    while (true) {
        block active_block = (this->tree).get_chain()[working_hash];
        if (active_block.p_hashes.size() == 1) {
            working_hash = *active_block.p_hashes.begin();
        } else {
            (this->constraint_path_fbs).insert(working_hash);
            for (auto p_hash : active_block.p_hashes) {
                if ((this->constraint_path_lbs).find(p_hash) == (this->constraint_path_lbs).end()) {
                    backscan_constraint_path(p_hash);
                }
            }
            break;
        }
    }
}

void Server::load_branch_forward(std::string fb_hash) {
    std::string working_hash = fb_hash; //start on the branch's first block
    block active_block;
    std::unordered_set<std::string> seed_hashes;

    if (((this->branches).count(fb_hash) > 0) || this->branches[fb_hash].first_hash.empty()) {
        for (auto c_fb : (this->branches)[fb_hash].c_branch_fbs) {
            (this->branches)[c_fb].p_branch_fbs.erase(fb_hash);
        }
    }
    else {
        (this->branches)[fb_hash].first_hash = fb_hash;
    }

    branch& target_branch = (this->branches)[fb_hash];

    target_branch.c_branch_fbs = std::unordered_set<std::string>();
    target_branch.messages = std::vector<message>();

    std::vector<branch_context> target_ctxs;

    for (auto ctx_branch_fb : target_branch.p_branch_fbs) {
        assert((this->branches).count(ctx_branch_fb) == 1);
        target_ctxs.push_back((this->branches)[ctx_branch_fb].ctx);
    }

    //merge contexts
    branch_context ctx(target_ctxs);

    target_branch.messages = std::vector<message>();

    //see how far the linear part goes; we'll stop when there are multiple children
    while (true) {
        active_block = (this->tree).get_chain()[working_hash];
        //message digestion logic
        try {
            std::array<std::string, 2> raw_unlocked = unlock_msg(b64_decode(active_block.cont), false, this->raw_AES_key);
            std::string content_hash = b64_encode(calc_hash(false, content_hash_concat(active_block.time, active_block.s_trip, active_block.p_hashes)));
            json claf_data = json::parse(raw_unlocked[0]);
            json extra;
            if (apply_data(ctx, extra, claf_data, raw_unlocked[0], raw_unlocked[1], content_hash)) {
                //add an actual message if we can
                message result;
                result.hash = active_block.hash;
                result.supertype = std::string(claf_data["st"]).at(0);
                result.type = std::string(claf_data["t"]).at(0);
                result.data = claf_data["d"];
                result.extra = extra;
                target_branch.messages.push_back(result);
            }
            else {
                throw; //failure to apply data
            }
        }
        catch(int err) {
            //something should go here
        }
        //see if we've reached a head, assuming there are any
        if (((this->constraint_heads).find(working_hash) != (this->constraint_path_fbs).end())) {
            //we're done with this branch - cutoff time!
            seed_hashes = std::unordered_set<std::string>(); //empty this
            break;
        }

        //see if we're still linear
        std::unordered_set<std::string> intraserver_c_hashes;
        for (auto c_hash : active_block.c_hashes) {
            if ((this->tree).get_chain()[c_hash].s_trip == (this->s_trip)) {
                intraserver_c_hashes.insert(c_hash);
            }
        }
        if (intraserver_c_hashes.size() == 1) {
            working_hash = *(intraserver_c_hashes.begin());
        } else {
            //no longer linear
            seed_hashes = intraserver_c_hashes;
            break;
        }
    }

    target_branch.ctx = ctx; //context is established now
    for (auto seed_hash : seed_hashes) {
        if (((this->constraint_heads).size() > 0) && ((this->constraint_path_fbs).find(seed_hash) == (this->constraint_path_fbs).end())) {
            //if there are any constraint heads, we need to make sure that this path will reach one of them.
            continue;
        }
        branch& seed_branch = (this->branches)[seed_hash];
        //if all the contexts are in, we can load the branch
        if (seed_branch.p_branch_fbs.size() == (size_t) (this->tree).intraserver_parent_count((this->tree).get_chain()[seed_hash])) {
            load_branch_forward(seed_hash);
        }
        target_branch.c_branch_fbs.insert(seed_branch.first_hash);
        seed_branch.p_branch_fbs.insert(target_branch.first_hash);
    }
}

void Server::add_block(std::string hash) {
    if ((this->constraint_heads).size() == 0) return; //if there are constraints, this should be static.
    std::string working_hash = hash;
    while (true) {
        std::unordered_set<std::string> intraserver_p_hashes;
        for (auto p_hash : (this->tree).get_chain()[working_hash].p_hashes) {
            if ((this->tree).get_chain()[p_hash].s_trip == (this->s_trip)) {
                intraserver_p_hashes.insert(p_hash);
            }
        }
        if (intraserver_p_hashes.size() == 1) {
            working_hash = *(intraserver_p_hashes.begin());
        }
        else {
            load_branch_forward(working_hash);
            break;
        }
    }
}

void Server::batch_add_blocks(std::unordered_set<std::string> hashes) {
    std::unordered_set<std::string> extern_only_hashes;
    for (auto h : hashes) {
        bool only_external = true;
        for (auto ph : (this->tree).get_chain()[h].p_hashes) {
            if (hashes.count(ph) != 0) only_external = false;
        }
        if (only_external) extern_only_hashes.insert(h);
    }
    for (auto eh : extern_only_hashes) {
        //loading forward from these hashes will get the full set
        add_block(eh);
    }
}

std::string Server::send_message(user author, json content, char st, std::string t, std::unordered_set<std::string> p_hashes) {
    assert((this->constraint_heads).size() == 0); //if there are constraints, this should be static.
    auto& local_tree = this->tree;
    auto sending_time = get_raw_time();
    std::unordered_set<std::string> target_p_hashes = local_tree.find_p_hashes(this->s_trip, p_hashes);
    std::string content_hash = b64_encode(calc_hash(false, content_hash_concat(sending_time, this->s_trip, target_p_hashes)));

    json full_msg = {
        {"a", author.u_trip},
        {"h", content_hash},
        {"st", std::string(1, st)},
        {"d", content},
    };

    if (!t.empty()) full_msg["t"] = t;

    std::string encrypted_content = b64_encode(lock_msg(full_msg.dump(), false, b64_decode(author.pri_keys.DSA_key), (this->raw_AES_key)));

    std::string target_hash = local_tree.gen_block(encrypted_content, this->s_trip, sending_time, target_p_hashes, author.u_trip);

    //add_block(target_hash);

    return target_hash;
}
