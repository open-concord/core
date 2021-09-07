/**
* Expiremental method for unifying server migrations across chains+rem commands

* When a rem command is called, any branches that aren't directly referened by either /
* the rem command or the parents/grandparents/etc of the rem command will be forgetten

* Conceptually, if you were to give a person rem perms, you'd trust them to include these branches' heads.
* But we at concord are paranoid, so we've included this system to nullify any possible censorship by the rem'r
*/

#include "../../inc/tree.h"
#include <vector>
#include <string>
#include <unordered_set>

std::unordered_set<std::string> _getHeads(Tree tree) {
  auto chain = tree.get_chain();
  // unordered vector of known heads
  std::vector<block> heads;
  // return set
  std::unordered_set<std::string> hashes;

  // push chain to vector
  for (std::pair<std::string, block> pblock : chain) {
    heads.push_back(pblock.second);
  }

  // remove parent blocks, leaving only children
  for (int i=0;i<heads.size();i++) {
    if (!heads.at(i).c_hashes.empty()) {
      heads.erase(heads.begin()+i);
    }
  }

  // translate vector of blocks back to vector of hashes
  for (int i=0;i<heads.size();i++) {
    hashes.insert(heads.at(i).hash);
  }
  // NEXT: Compare the user's heads to the server's heads
  return hashes;
}

std::unordered_set<std::string> _compareHeadRecords(Tree tree, std::unordered_set<std::string> foward) {
  // get user's heads
  std::unordered_set<std::string> heads = _getHeads(tree);
  // compare against server's heads
  for (const auto& phead : heads) {
    // check if phead exists in foward
    if (foward.count(phead)) heads.erase(phead);
  }
  // NEXT: publish remaining heads to server
  return heads;
}
