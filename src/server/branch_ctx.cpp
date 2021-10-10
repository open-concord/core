#include "../../inc/server.h"
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>

void birank::orient_dir(bool dir) {
    //do nothing if it's already in the right direction
    bool curr = get_dir();
    if (curr != dir) {
        //positive gets increased before being flipped
        //negative just gets flipped
        if (curr) {
            this->irank += 1;
        }
        this->irank *= -1;
    }
}

void birank::increment() {
    if (get_dir()) {
        this->irank++;
    } else {
        this->irank--;
    }
}

bool birank::get_dir() {
    return std::abs(this->irank) >= 0;
    //0, the initial value, is considered negative. This gives negatives preference (base negative, -1, has higher rank)
}

birank::operator bool() {
    return get_dir();
}

bool operator==(birank b1, birank b2) {
    return (b1.irank == b2.irank);
}

bool operator>(birank b1, birank b2) {
    int abs_r1 = std::abs(b1.irank);
    int abs_r2 = std::abs(b2.irank);
    if (abs_r1 > abs_r2) {
        return true;
    } else if (abs_r1 == abs_r2) {
        return ((abs_r1 >= 0) && (abs_r2 < 0));
        //positive gets priority because 0 is treated as such.
        //>= 0 is used here, but they'll never be equal and opposite in that case.
    } else {
        return false;
    }
}

bool operator<(birank b1, birank b2) {
    return (b2 > b1);
}

bijson::bijson() {

}

bijson::bijson(json base) {
    set_base(base);
}

void bijson::set_type(bijson_type new_type) {
    this->type = new_type;
    if (new_type != TERMINAL) this->base_json = {};
    if (new_type != MAP) (this->map_values).clear();
    if (new_type != VECT) (this->vect_values).clear();
}

void bijson::set_base(json new_base) {
    set_type(TERMINAL);
    this->base_json = new_base;
}

void bijson::set_key(std::string key, bijson value) {
    set_type(MAP);
    auto& t_bi = (this->map_values)[key];
    t_bi.second.orient_dir(false); //effectively clear before setting
    t_bi.second.orient_dir(true);
    t_bi.first = value;
}

void bijson::clear_key(std::string key) {
    set_type(MAP);
    auto& t_bi = (this->map_values)[key];
    t_bi.first = bijson();
    t_bi.second.orient_dir(false);
}

void bijson::set_keys(std::map<std::string, bijson> key_values)  {
    for (auto& [key, value] : key_values) {
        set_key(key, value);
    }
}

void bijson::clear_keys(std::vector<std::string> keys)  {
    for (auto& key : keys) {
        clear_key(key);
    }
}

/*
void bijson::set_indices(std::map<size_t, bijson> index_values)  {
    set_type(VECT);
    for (auto& [index, value] : index_values) {
        size_t vect_size = (this->vect_values).size();
        assert (index < vect_size + 1); //don't do this!
        if (index == vect_size) {
            std::pair<bijson, birank> new_elem;
            new_elem.first = value;
            (this->vect_values).push_back(new_elem);
        } else {
            (this->vect_values)[index].first = value;
            auto& t_bi_rank = (this->vect_values)[index].second;
            t_bi_rank.orient_dir(true);
            t_bi_rank.increment();
        }
    }
}

void bijson::clear_indices(std::vector<size_t> indices)  {
    set_type(VECT);
    size_t vect_size = (this->vect_values).size();
    for (auto& index : indices) {
        assert(index < vect_size);
        auto& t_bi_rank = (this->vect_values)[index].second;
        t_bi_rank.orient_dir(false);
    }
}
//*/

bijson merge(bijson bs1, bijson bs2) {
    assert(bs1.type == bs2.type);
    bijson_type merge_type = bs1.type;
    bijson bsm;
    bsm.set_type(merge_type);
    
    if (merge_type == MAP) {
        for (auto& [key1, value1] : bs1.map_values) {
            auto& value1_m = bsm.map_values[key1];
            value1_m = value1;
        }
        for (auto& [key2, value2] : bs2.map_values) {
            if (bsm.map_values.count(key2) == 0) bsm.map_values[key2] = value2;
            else {
                auto& value2_m = bsm.map_values[key2];
                if (value2.second > value2_m.second) {
                    value2_m = value2;
                }
                else if (
                    value2.first.type == MAP &&
                    value2_m.first.type == MAP && 
                    value2.second.get_dir() && 
                    value2.second == value2_m.second
                ) {
                    value2_m.first = merge(value2_m.first, value2.first);
                }
            }
        }
    }
    return bsm;
}

void branch_context::initialize_roles()  {
    //by default, the creator role exists.
    role creator_role;
    creator_role.features[0].orient_dir(false);
    for (size_t i = 1; i < creator_role.features.size(); i++) creator_role.features[i].orient_dir(true);
    creator_role.primacy_rank = {0, 0};
    (this->roles)["creator"] = creator_role;
}

branch_context::branch_context() {
    this->settings.set_type(MAP);
    initialize_roles();
}

branch_context::branch_context(std::vector<branch_context> input_contexts) {
    this->settings.set_type(MAP);
    for (auto input_context : input_contexts) {
        //rank selections for equal rank are random.
        //they can also be easily manipulated by repeated definition
        //this is of no consequence; the point is that action is chosen over inaction
        //all of these are privileges given to high-ranking users, and all of them can easily be removed if (to trivial and reversible effect) abused

        //make the highest-rank-magnitude role selections and simultaneously get union of membership
        for (const auto& [hash, in_member] : input_context.members) {
            if ((this->members).count(hash) == 0) {
                (this->members)[hash] = in_member;
            } 
            else {
                for (const auto& [name, rank] : in_member.roles_ranks) {
                    (this->members)[hash].roles_ranks[name] = std::max(rank, (this->members)[hash].roles_ranks[name]);
                }
            }
        }

        //also select role features by rank - same procedure
        for (const auto& [name, in_role] : input_context.roles) {
            if ((this->roles).count(name) == 0) {
                (this->roles)[name] = in_role;
            } 
            else {
                for (size_t i = 0; i < in_role.features.size(); i++) {
                    (this->roles)[name].features[i] = std::max(in_role.features[i], (this->roles)[name].features[i]);
                }
            }
        }
        
        this->settings = merge(this->settings, input_context.settings);
    }
}

unsigned int branch_context::min_primacy(member target) {
    unsigned int min = std::numeric_limits<int>::max();
    for (auto name_pair : target.roles_ranks) {
        if (name_pair.second < 0) continue;
        unsigned int role_primacy = ((this->roles)[name_pair.first]).primacy();
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
        bool role_feature = ((this->roles)[name_pair.first]).features[index];
        result = (result || role_feature);
    }
    return result;
}