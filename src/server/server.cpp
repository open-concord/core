#include "../../tree.h"
#include "../../hash.h"
#include "../../strenc.h"
#include "../../crypt++.h"

#include <map>
#include <unordered_set>
#include <vector>
#include <array>
#include <nlohmann/json.hpp>
#include <boost/bind/bind.hpp>


using json = nlohmann::json;

Server::Server(Tree& parent_tree, std::string AES_key, user load_user) : tree(parent_tree) {
    this->raw_AES_key = b64_decode(AES_key);
    this->s_trip = gen_trip(AES_key, 24);
    std::unordered_set<std::string> root_hashes = this->tree.get_qualifying_hashes(boost::bind(&Tree::is_intraserver_orphan, _1, _2, this->s_trip));
    assert(root_hashes.size() == 1); //0 means the server doesn't exist. 2+ shouldn't be possible, as there are checks at every level for server connection.
    this->root_fb = *(root_hashes.begin());

    load_branch_forward(this->root_fb);
}

member create_member(keypair pub_keys, std::vector<std::string> initial_roles) {
    user temp_user(pub_keys);
    (this->known_users)[temp_user.u_trip] = temp_user;
    member temp_member;
    temp_member.user_ref = &((this->known_users)[temp_user.u_trip]);
    temp_member.roles = initial_roles;
    return temp_member;
}

void load_branch_forward(std::string fb_hash, branch_context ctx) {
    block active_block = tree.get_chain()[fb_hash]; //start on the branch's first block

    while (tree.intraserver_child_count(active_block) == 0) {
        //message digestion logic
        try {
            std::array<std::string, 2> raw_unlocked = unlock_msg(active_block.cont, false, this->raw_AES_key);
            json claf_data = json::parse(raw_unlocked[0]);
            assert(content_hash_concat(active_block.time, active_block.s_trip, active_block.p_hashes) == (std::string) claf_data["h"]);

            //nserv needs to be checked before membership
            if (ctx.members.empty() && claf_data["st"] == "a" && claf_data["t"] == "nserv") {
                keypair creator_pubset(claf_data["d"]["cms"]["sig_pubk"], claf_data["d"]["cms"]["enc_pubk"]);
                member temp_member = create_member(creator_pubset, std::vector<std::string>({"creator"}));
                ctx.members[(*member.user_ref).u_trip] = temp_member;
            }

            //make sure this is properly signed by an actual member
            member author_member = ctx.members[claf_data["a"]]
            user author = *(author_member.user_ref);
            std::string author_raw_sigkey = author.pub_keys.DSA_key;
            assert(DSA_verify(author_raw_sigkey, raw_unlocked[1], active_block.cont));

            if (claf_data["st"] == "c") {
                for (const auto& [name, role] : member.roles) {
                    assert(!role.is_muted);
                }
            } 
            else if (claf_data["st"] == "a") {
                if (claf_data["t"] == "invite") {
                    std::vector<json> keysets = claf_data["d"]["nms"];
                    for (auto keyset : keysets) {
                        keypair nm_pubset(keyset["sig_pubk"], keyset["enc_pubk"]);
                        member temp_member = create_member(nm_pubset);
                        ctx.members[(*member.user_ref).u_trip] = temp_member;
                    }
                }
                else if (claf_data["t"] == "rem") {
                    //TODO: add this beast
                }
            }
            else if (claf_data["st"] == "r") {
                if (claf_data["t"] == "crole") {
                    //TODO: conversion methods in the works
                }
                else if (claf_data["t"] == "grole") {
                    ctx.members[claf_data["d"]["tu"]].roles.push_back(claf_data["d"]["tr"]);
                }
                else if (claf_data["t"] == "rrole") {
                    member& member_ref = ctx.members[claf_data["d"]["tu"]];
                    std::remove(member_ref.roles.begin(), member_ref.end(), claf_data["d"]["tr"]);
                }
            }
            else if (claf_data["st"] == "s") {
                json& target;
                for (auto index : std::vector<std::string>(claf_data["d"]["sn"])) {
                    target = target[index];
                }
                target = claf_data["d"]["sv"];
            }
        }
    }
}
