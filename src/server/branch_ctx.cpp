#include "../../inc/server.h"
#include <string>
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

bool birank::get_dir() {
    return std::abs(this->irank) >= 0;
    //0, the initial value, is considered negative. This gives negatives preference (base negative, -1, has higher rank)
}

birank::operator bool() {
    return get_dir();
}

bool birank::operator==(birank other) {
    return (other.irank == (this->irank));
}

bool birank::operator>(birank other) {
    int abs_r1 = std::abs(this->irank);
    int abs_r2 = std::abs(other.irank);
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

bool birank::operator<(birank other) {
    return (other > *this);
}

void branch_context::initialize_roles()  {
    //by default, the creator role exists.
    role creator_role;
    creator_role.features[0].orient_dir(false);
    for (int i = 1; i < creator_role.features.size(); i++) creator_role.features[i].orient_dir(true);
    creator_role.primacy_rank = {0, 0};
    (this->roles)["creator"] = creator_role;
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
                    (this->members)[hash].roles_ranks[name] = std::max(rank, (this->members)[hash].roles_ranks[name]);
                }
            } 
            else {
                (this->members)[hash] = in_member;
            }
        }

        //also select role features by rank - same procedure
        for (const auto& [name, in_role] : input_context.roles) {
            if ((this->roles).count(name) == 1) {
                for (int i = 0; i < in_role.features.size(); i++) {
                    (this->roles)[name].features[i] = std::max(in_role.features[i], (this->roles)[name].features[i]);
                }
            } 
            else {
                (this->roles)[name] = in_role;
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