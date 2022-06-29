#pragma once
#include "../../inc/claf.hpp"

bool apply_error(std::string tag, std::string context) {
  std::cout << "[Server::apply_data | "+tag+"] "+context+'\n';
  return false;
};

/** oh god */

bool Server::apply_data(
    branch_context& ctx,
    json& extra,
    json claf_data,
    std::string content,
    std::string signature,
    std::string content_hash
    ) {

    // verify that all of the circumstances agree with those in the block
    if (content_hash != std::string(claf_data["h"])) {
      // [ERROR]
      return apply_error("hash", "Content hash doesn't match");
    }

    // nserv needs to be checked before membership
    if (ctx.members.empty() && claf_data["st"] == "a" && claf_data["t"] == "nserv") {
        keypair creator_pubset(claf_data["d"]["cms"]["sig_pubk"], claf_data["d"]["cms"]["enc_pubk"]);
        member temp_member = create_member(creator_pubset, std::vector<std::string>({"creator"}));
        ctx.members[temp_member.user_trip] = temp_member;
        return true;
    }

    //make sure this is properly signed by an actual member
    member author_member = ctx.members[claf_data["a"]];
    user author = (this->known_users)[author_member.user_trip];
    std::string author_sigkey = author.pubkeys.DSA;
    unsigned int author_primacy = ctx.min_primacy(author_member);
    if (!cDSA::verify(b64::decode(author_sigkey), signature, content)) {
      // [ERROR]
      return apply_error("signature", "Signature is not valid");
    }

    if (claf_data["st"] == "c") {
        /*if (ctx.has_feature(author_member, 0)) {
            std::cout << "author muted\n";
            return false; //0 is is_muted
        }*/
    }
    else if (claf_data["st"] == "a") {
        if (claf_data["t"] == "invite") {
            if (!ctx.has_feature(author_member, 1)) { // 1 => can_invite
              // [ERROR]
              return apply_error("invite", "User cannot invite others");
            }
            std::vector<json> keysets = claf_data["d"]["nms"];
            for (auto keyset : keysets) {
                keypair nm_pubset(keyset["sig_pubk"], keyset["enc_pubk"]);
                member temp_member = create_member(nm_pubset);
                ctx.members[temp_member.user_trip] = temp_member;
            }
        }
        else if (claf_data["t"] == "rem") {
            if (!ctx.has_feature(author_member, 2)) { // 2 => can_rem
              // [ERROR]
              return apply_error("rem", "User does not have rem permissions"); 
            }
            std::unordered_set<std::string> nsk_indices;
            for (auto& nsk_entry : (claf_data["d"]["nsk"]).items()) {
                nsk_indices.insert(nsk_entry.key());
            }
            std::unordered_set<std::string> relevant_members;
            std::vector<std::string> rem_users = claf_data["d"]["rms"];
            //get unremoved members + make sure all are there
            for (auto& [user_trip, member] : ctx.members) {
                if (std::find(rem_users.begin(), rem_users.end(), user_trip) == rem_users.end()) {
                    if (!claf_data["d"]["nsk"].contains(user_trip)) {
                      // [ERROR]
                      return apply_error(" ", "CLAF doesn't contain all members");
                    }
                    relevant_members.insert(user_trip);
                }
            }
            if (relevant_members.find((this->luser).trip) == relevant_members.end()) { // luser is not a member
              // [ERROR]
              return apply_error("user", "User is not a member");
            }
            
            std::string encrypted_key = b64::decode(claf_data["d"]["nsk"][(this->luser).trip]);
            std::string decrypted_key = cRSA::decrypt(b64::decode(luser.prikeys.RSA), encrypted_key);
            std::string nserver_trip = gen::hash(false, decrypted_key);
            if (nserver_trip != claf_data["d"]["nst"]) {
              // [ERROR]
              // make sure this is the right key
              return apply_error("key", "Key doesn't match"); 
            }
            for (auto& [user_trip, member] : ctx.members) { //first verify all members are included
                std::string user_rsa_pubk = (this->known_users)[user_trip].pubkeys.RSA;
                //make sure they have the right ciphertext for every other user as well
                if (b64::encode(
                      cRSA::encrypt(
                        b64::decode(user_rsa_pubk),
                        b64::decode(decrypted_key)
                      )
                    ) != claf_data["d"]["nsk"][user_trip]
                ) {
                  // [ERROR]
                  return apply_error("legit", "Could not verifiy (other) member\'s authenticity");
                }
            }
            //now we know that everything is valid, so a new server is in order.
            extra["s_key"] = decrypted_key;
        }
        else return false;
    }
    else if (claf_data["st"] == "r") {
        if (claf_data["t"] == "crole") {
            if (!ctx.has_feature(author_member, 4)) { // 4 => role_creation
              // [ERROR]
              return apply_error("role", "User doesn\'t have RoleCreation permissions");
            }
            std::string target_role = claf_data["d"]["rn"];
            unsigned int target_primacy = claf_data["d"]["rp"];
            std::array<bool, 6> target_features = features::decode(claf_data["d"]["pc"]);
            bool create_role = (ctx.roles.count(target_role) == 0); //we need to create it before we act on this
            auto& present_role = ctx.roles[target_role];
            if (author_primacy >= target_primacy) {
              // [ERROR]
              return apply_error("primacy", "User cannot scale further role"); 
            }
            if (create_role) {
                present_role.primacy_rank = {target_primacy, 0};
            } else if (present_role.get_primacy() != target_primacy) {
                present_role.primacy_rank[0] = target_primacy;
                present_role.primacy_rank[1]++;
            }
            for (size_t i = 0; i < target_features.size(); i++) {
                present_role.features[i].orient_dir(target_features[i]);
            }
        } else {
            if (!ctx.has_feature(author_member, 3)) { // 3 => grole/rrole
              // [ERROR]
              return apply_error("(g/r)role", "User cannot give/remove roles"); 
            }
            auto& altered_member = ctx.members[claf_data["d"]["tu"]];
            std::string target_role = claf_data["d"]["tr"];
            if (author_primacy >= ctx.roles[target_role].get_primacy()) {
              // [ERROR]
              // need to be more prime (lol)
              return apply_error("target.primacy", "User cannot modify higher role"); 
            }
            bool direction;
            if (claf_data["t"] == "grole") {
                direction = true;
            }
            else if (claf_data["t"] == "rrole") {
                direction = false;
            }
            else return false;

            altered_member.roles_ranks[target_role].orient_dir(direction);
        }
    }
    else if (claf_data["st"] == "s") {
        if (!ctx.has_feature(author_member, 5)) { // 5 => can_edit 
          // [ERROR]
          return apply_error("can_edit", "User cannot edit");
        }
        bool is_isset = (claf_data["t"] == "sset");
        bool is_cset = (claf_data["t"] == "cset");

        if (!is_isset && !is_cset) {
          // [ERROR]
          return false;
        }
        std::vector<std::vector<std::string>> key_vects = claf_data["d"]["sn"];
        std::vector<int> po_bools = claf_data["d"]["po"];
        std::vector<json> vals = claf_data["d"]["sv"];
        if (is_isset && key_vects.size() != vals.size()) {
          // [ERROR]
          return false;
        }
        if (po_bools.size() == 0) po_bools = std::vector<int>(1, key_vects.size());
        else if (po_bools.size() != key_vects.size()) {
          // [ERROR]
          return false;
        }
        for (size_t i = 0; i < key_vects.size(); i++) {
            bijson* moving_ref = &ctx.settings;
            std::vector<std::string> keys = key_vects[i];
            std::string last_key = keys.back();
            keys.pop_back();
            for (auto key : keys) {
                auto& val_map = (*moving_ref).map_values;
                if (val_map.count(key) == 0) {
                  // [ERROR]
                  return false;
                }
                if (!val_map[key].second.get_dir()) {
                  // [ERROR]
                  return false;
                }
                moving_ref = &val_map[key].first;
            }
            if (is_isset) {
              bijson end_setting(vals[i]);
              (*moving_ref).set_key(last_key, end_setting);
            } else {
              (*moving_ref).clear_key(last_key);
            }
        }

    }
    else return false;

    return true;
}
