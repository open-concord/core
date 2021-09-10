#include "../../inc/server.h"
#include "../../inc/tree.h"
#include <string>
#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string content_hash_concat(std::string time, std::string s_trip, std::unordered_set<std::string> p_hashes) {
    std::string concat_data = b64_encode(raw_time_to_string(time)) + s_trip; //b64 timestr encoding is only for safety
    for (auto ph : order_hashes(p_hashes)) concat_data += ph;
    return concat_data;
}

bool Server::apply_data(branch_context& ctx, json claf_data, std::string content, std::string signature, std::string content_hash) {

    //verify that all of the circumstances agree with those in the block
    if (content_hash != std::string(claf_data["h"])) return false; 

    //nserv needs to be checked before membership
    if (ctx.members.empty() && claf_data["st"] == "a" && claf_data["t"] == "nserv") {
        keypair creator_pubset(claf_data["d"]["cms"]["sig_pubk"], claf_data["d"]["cms"]["enc_pubk"]);
        member temp_member = create_member(creator_pubset, std::vector<std::string>({"creator"}));
        ctx.members[(*member.user_ref).u_trip] = temp_member;
    }

    //make sure this is properly signed by an actual member
    member author_member = ctx.members[claf_data["a"]];
    user author = *(author_member.user_ref);
    std::string author_raw_sigkey = author.pub_keys.DSA_key;
    unsigned int author_primacy = ctx.min_primacy(author_member);
    if (!DSA_verify(author_raw_sigkey, signature, content)) return false;

    if (claf_data["st"] == "c") {
        if (ctx.has_feature(author_member, 0)) return false; //0 is is_muted
    } 
    else if (claf_data["st"] == "a") {
        if (claf_data["t"] == "invite") {
            if (!ctx.has_feature(author_member, 1)) return false; //1 is can_invite
            std::vector<json> keysets = claf_data["d"]["nms"];
            for (auto keyset : keysets) {
                keypair nm_pubset(keyset["sig_pubk"], keyset["enc_pubk"]);
                member temp_member = create_member(nm_pubset);
                ctx.members[(*member.user_ref).u_trip] = temp_member;
            }
        }
        else if (claf_data["t"] == "rem") {
            if (!ctx.has_feature(author_member, 2)) return false; //2 is can_rem
            //TODO: add this beast
        }
    }
    else if (claf_data["st"] == "r") {
        if (claf_data["t"] == "crole") {
            //TODO: conversion methods in the works
        }
        else {
            if (!ctx.has_feature(author_member, 3)) return false; //3 is grole/rrole
            if (claf_data["t"] == "grole") {
                ctx.members[claf_data["d"]["tu"]].roles.push_back(claf_data["d"]["tr"]);
            }
            else if (claf_data["t"] == "rrole") {
                member& member_ref = ctx.members[claf_data["d"]["tu"]];
                std::remove(member_ref.roles.begin(), member_ref.end(), claf_data["d"]["tr"]);
            }
        }
    }
    else if (claf_data["st"] == "s") {
        if (!ctx.has_feature(author_member, 5)) return false; //5 is can_edit
        json& target;
        for (auto index : std::vector<std::string>(claf_data["d"]["sn"])) {
            target = target[index];
        }
        target = claf_data["d"]["sv"];
    }

    return true;
}