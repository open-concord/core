#include "../../inc/server.hpp"
#include "../../inc/tree.hpp"
#include "../../inc/crypt++.hpp"
#include "../../inc/strops.hpp"

#include <string>
#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int encode_features(std::array<bool, 6> features) {
    int output = 0;
    for (size_t i = 0; i < features.size(); i++) {
        output += i;
        output <<= 1;
    }
    return output;
}

std::array<bool, 6> decode_features(int encoded_features) {
    std::array<bool, 6> output;
    int moving_filter = 0;
    for (size_t i = 0; i < output.size(); i++) {
        moving_filter <<= 1;
        output[i] = (encoded_features & moving_filter);
    }
    return output;
}

bijson json_to_bij(json input) {
    bijson result;
    if (input.is_object()) {
        for (auto& [key, val] : input.items()) {
            result.set_key(key, json_to_bij(val));
        }
    } else {
        result.set_base(input);
    }
    return result;
}

json bij_to_json(bijson input) {
    json result;
    if (input.type == MAP) {
        for (auto& [key, val] : input.map_values) {
            if (val.second.get_dir()) {
                result[key] = bij_to_json(val.first);
            }
        }
    } else {
        result = input.base_json;
    }
    return result;
}

std::string content_hash_concat(long long unsigned int time, std::string s_trip, std::unordered_set<std::string> p_hashes) {
    std::string concat_data = b64_encode(raw_time_to_string(time)) + s_trip; //b64 timestr encoding is only for safety
    for (auto ph : order_hashes(p_hashes)) concat_data += ph;
    return concat_data;
}

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
      std::cout << "[Server::apply_data|h] Content hash doesn't match\n";
      return false;
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
    std::string author_sigkey = author.pub_keys.DSA_key;
    unsigned int author_primacy = ctx.min_primacy(author_member);
    if (!DSA_verify(b64_decode(author_sigkey), signature, content)) {
      // [ERROR]
      std::cout << "[Server::apply_data|signature] Signature is not valid\n";
      return false;
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
              std::cout << "[Server::apply_data|invite] User cannot invite others\n";
              return false;
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
              std::cout << "[Server::apply_data|rem] User does not have rem permissions\n";
              return false;
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
                      std::cout << "[Server::apply_data] CLAF doesn't contain all members\n";
                      return false;
                    }
                    relevant_members.insert(user_trip);
                }
            }
            if (relevant_members.find((this->luser).u_trip) == relevant_members.end()) { // luser is not a member
              // [ERROR]
              std::cout << "[Server::apply_data|user] User is not a member\n";
              return false;
            }
            
            std::string encrypted_key = b64_decode(claf_data["d"]["nsk"][(this->luser).u_trip]);
            std::string decrypted_key = RSA_decrypt(b64_decode(luser.pri_keys.RSA_key), encrypted_key);
            std::string nserver_trip = calc_hash(false, decrypted_key);
            if (nserver_trip != claf_data["d"]["nst"]) {
              // [ERROR]
              std::cout << "[Server::apply_data|key] Key doesn't match\n";
              return false; //make sure this is the right key
            }
            for (auto& [user_trip, member] : ctx.members) { //first verify all members are included
                std::string user_rsa_pubk = (this->known_users)[user_trip].pub_keys.RSA_key;
                //make sure they have the right ciphertext for every other user as well
                if (b64_encode(
                      RSA_encrypt(
                        b64_decode(user_rsa_pubk),
                        b64_decode(decrypted_key)
                      )
                    ) != claf_data["d"]["nsk"][user_trip]
                ) {
                  // [ERROR]
                  std::cout << "[Server::apply_data|legit] Could not verifiy (other) member\'s authenticity\n";
                  return false;
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
              std::cout << "[Server::apply_data|role] User doesn\'t have RoleCreation permissions\n";
              return false;
            }
            std::string target_role = claf_data["d"]["rn"];
            unsigned int target_primacy = claf_data["d"]["rp"];
            std::array<bool, 6> target_features = decode_features(claf_data["d"]["pc"]);
            bool create_role = (ctx.roles.count(target_role) == 0); //we need to create it before we act on this
            auto& present_role = ctx.roles[target_role];
            if (author_primacy >= target_primacy) {
              // [ERROR]
              std::cout << "[Server::apply_data|primacy] User cannot scale further role\n";
              return false;
            }
            if (create_role) {
                present_role.primacy_rank = {target_primacy, 0};
            } else if (present_role.primacy() != target_primacy) {
                present_role.primacy_rank[0] = target_primacy;
                present_role.primacy_rank[1]++;
            }
            for (size_t i = 0; i < target_features.size(); i++) {
                present_role.features[i].orient_dir(target_features[i]);
            }
        } else {
            if (!ctx.has_feature(author_member, 3)) { // 3 => grole/rrole
              // [ERROR]
              std::cout << "[Server::apply_data|(g/r)role] User cannot give/remove roles\n";
              return false;
            }
            auto& altered_member = ctx.members[claf_data["d"]["tu"]];
            std::string target_role = claf_data["d"]["tr"];
            if (author_primacy >= ctx.roles[target_role].primacy()) {
              // [ERROR]
              std::cout << "[Server::apply_data|target.primacy] User cannot modify higher role\n";
              return false; //need to be more prime
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
          std::cout << "[Server::apply_data|can_edit] User cannot edit\n";
          return false;
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
                bijson end_setting = (po_bools[i] == 1) ? json_to_bij(vals[i]) : bijson(vals[i]);
                (*moving_ref).set_key(last_key, end_setting);
            } else {
                (*moving_ref).clear_key(last_key);
            }
        }

    }
    else return false;

    return true;
}
