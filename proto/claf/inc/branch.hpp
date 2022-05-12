#pragma once

#include "bijson.hpp"
#include "content.hpp" 
#include <string>
#include <map>

class branch_context {
  private:
    void initialize_roles();
  public:
    bijson settings;
    std::map<std::string, member> members;
    std::map<std::string, role> roles;

    branch_context();
    branch_context(std::vector<branch_context> input_contexts);

    unsigned int min_primacy(member target);

    bool has_feature(member target, int index);
};

struct branch {
  std::string first_hash;
  std::vector<message> messages;
  branch_context ctx;

  std::unordered_set<std::string> c_branch_fbs;
  std::unordered_set<std::string> p_branch_fbs;
};
