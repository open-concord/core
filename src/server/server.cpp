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
        try {
            std::array<std::string, 2> raw_unlocked = unlock_msg(active_block.cont, false, this->raw_AES_key);
            json claf_data = json::parse(raw_unlocked[0]);
            assert(content_hash_concat(active_block.time, active_block.s_trip, active_block.p_hashes) == (std::string) claf_data["h"]);
            if (claf_data["type"] == "nserv" && ctx.members.empty()) {
                keypair creator_pubset(claf_data["d"]["cms"]["sig_pubk"], claf_data["d"]["cms"]["enc_pubk"]);
                ctx.members.insert(create_member(creator_pubset, std::vector<std::string>({"creator"})));
            }
        }
    }
}
