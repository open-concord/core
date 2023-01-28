#include "../../inc/tree.hpp"
#include <algorithm>


Tree::Tree() {}

void 
Tree::graph_configure(block root) {
  // for now, just extract POW threshold
  json config = json::parse(root.cont);
  if (config.contains("pow")) set_pow_req((int) config["pow"]);
}

void 
Tree::set_pow_req(int POW_req) {
  bool increased = (this->pow) < POW_req;
  this->pow = POW_req;
  if (!increased) return;
    
  // blocks could be made invalid, so we need to re-build the entire tree.
  std::lock_guard lk(this->push_proc_mtx);
  std::unordered_set<block> known_blocks;
  for (const auto& [hash, l_block] : get_graph()) known_blocks.insert(l_block.ref);
    
  (this->graph).clear();
  std::queue<std::unordered_set<block>>().swap((this->awaiting_push_batches));
  batch_push(known_blocks);
}

int 
Tree::get_pow_req() {return this->pow;}

std::string 
Tree::gen_block(
  std::string cont,
  std::string s_trip,
  unsigned long long set_time,
  std::unordered_set<std::string> p_hashes,
  std::string c_trip
  ) {
  assert(s_trip.length() == 24);
  assert(c_trip.length() == 24 || c_trip.length() == 0);
  if (p_hashes.size() == 0) p_hashes = find_p_hashes(s_trip);
  block out_block(cont, p_hashes, this->pow, s_trip, set_time, c_trip);
  queue_unit(out_block);
  return out_block.hash;
}

void 
Tree::create_root() {
  assert(this->get_graph().empty());
  json root_msg;
  root_msg["pow"] = this->pow;
  this->gen_block(root_msg.dump(), std::string(24, '='));
}

std::unordered_set<block> 
Tree::get_valid(std::unordered_set<block> to_check) {
  bool root_found = check_rooted();
  std::unordered_set<std::string> rooted_servers;
  for (const auto& [s_trip, root] : (this->server_roots)) rooted_servers.insert(s_trip);

  std::map<std::string, std::string> s_trip_by_hash;
  for (const auto& tc_block : to_check) s_trip_by_hash[tc_block.hash] = tc_block.s_trip;

  for (const auto& tc_block : to_check) {
    if (!tc_block.verify(get_pow_req())) {
      to_check.erase(tc_block);
      continue;
    }

    if (is_orphan(tc_block.hash)) {
      if (root_found) to_check.erase(tc_block);
      else root_found = true;
    }

    bool intra_orphan  = true;
    for (const auto& p_hash : tc_block.p_hashes) {
      if ((
            s_trip_by_hash.contains(p_hash) 
            && s_trip_by_hash[p_hash] == tc_block.s_trip
          ) || (
            get_graph().contains(p_hash) 
            && get_graph()[p_hash].ref.s_trip == tc_block.s_trip
          )) intra_orphan = false;
    }

    if (intra_orphan) {
      if (rooted_servers.contains(tc_block.s_trip)) to_check.erase(tc_block);
      else rooted_servers.insert(tc_block.s_trip);
    }
  }
  
  return to_check;
}

/**
 * FIXME
 * Why do we take flags as an unordered set here?
 * We only check 'no-save', and we don't pass the flags to anywhere else
 * If we do need to take these flags, this should be a bit-mask; an unordered set of strings is a lot of memory for this function.
 * If we don't need to take these flags, this should just be a bool.
 */
void 
Tree::push_response(
    std::unordered_set<std::string> new_trips, 
    std::unordered_set<std::string> flags 
  ) {
  bool save_new = !flags.contains("no-save"); 
  std::map<std::string, std::unordered_set<std::string>> server_batches;
  for (const auto& new_trip : new_trips) {
    block new_block = get_graph()[new_trip].ref;

    if (is_intraserver_orphan(new_trip)) 
      (this->server_roots)[new_block.s_trip] = &((this->graph)[new_trip]);
    if (save_new) save(new_block);

    server_batches[new_block.s_trip].insert(new_trip);
  }

  for (const auto& [s_trip, batch] : server_batches) server_add_funcs[s_trip](batch);
}
