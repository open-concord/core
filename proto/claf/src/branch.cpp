#include "../inc/claf.hpp"

void branch_context::initialize_roles() {
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
  /**
   * rank selections for equal rank are random.
   * they can also be easily manipulated by repeated definition
   * this is of no consequence; the point is that action is chosen over inaction
   * all of these are privileges given to high-ranking users, and all of them can easily be removed if (to trivial and reversible effect) abused
   * make the highest-rank-magnitude role selections and simultaneously get union of membership
  */  
  for (const auto& [hash, in_member] : input_context.members) {
    if ((this->members).count(hash) == 0) { 
      (this->members)[hash] = in_member;
    } else {
      for (const auto& [name, rank] : in_member.roles_ranks) {
        (this->members)[hash].roles_ranks[name] = std::max(rank, (this->members)[hash].roles_ranks[name]);
      }
    }
  }
        // also select role features by rank - same procedure
  for (const auto& [name, in_role] : input_context.roles) {
    if ((this->roles).count(name) == 0) {
      (this->roles)[name] = in_role;
    } else {
      for (size_t i = 0; i < in_role.features.size(); i++) {
        (this->roles)[name].features[i] = std::max(in_role.features[i], (this->roles)[name].features[i]);
      }
    }
  }

  this->settings = bijson(this->settings, input_context.settings);
  }
}

unsigned int branch_context::min_primacy(member target) {
  unsigned int min = std::numeric_limits<int>::max();
  for (auto name_pair : target.roles_ranks) {
    if (name_pair.second < 0) continue;
    unsigned int role_primacy = ((this->roles)[name_pair.first]).get_primacy();

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
    role target_role = (this->roles)[name_pair.first];
    for (auto& feat : target_role.features) {
      std::cout << name_pair.first << " has " << feat.irank << "\n";
    }
    birank role_feature = ((this->roles)[name_pair.first]).features[index];
    std::cout << "role rank for " << name_pair.first << " is " << role_feature.irank << "\n";
    result = (result || (bool) role_feature);
  }
  return result;
}
