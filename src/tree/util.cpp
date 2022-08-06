#include "../../inc/tree.hpp"
#include <experimental/algorithm>
#include <random>

std::unordered_set<std::string> Tree::find_p_hashes(std::string s_trip, std::unordered_set<std::string> base_p_hashes, int p_count) {
    /**
    * there's no point in using blocks with existing children as hashes;
    * we can get the same reliance by using their children
    */
    std::unordered_set<std::string> p_hashes = base_p_hashes;

    std::unordered_set<std::string> intra_childless_hashes = get_qualifying_hashes(&Tree::is_childless);

    /**
    * if the base hashes have an intraserver block,
    * we don't require another one, so the minimum intra sample is 0.
    * Otherwise, it's 1 for server continuity.
    */
    bool require_intra_block = true;

    for (const auto& bp_hash : p_hashes) {
        if (get_chain()[bp_hash].ref.s_trip == s_trip) {
            require_intra_block = false;
            break;
        }
    }

    std::sample(
        intra_childless_hashes.begin(),
        intra_childless_hashes.end(),
        std::inserter(p_hashes, p_hashes.begin()),
        std::max(p_count - (int) p_hashes.size(), (int) require_intra_block), //we want at least one block from the server for continuity, but if we already have one we just need to fill the p_count.
        std::mt19937{std::random_device{}()}
    );
    int p_remainder = p_count - (int) p_hashes.size();
    if (p_remainder > 0) {
        std::unordered_set<std::string> childless_hashes = get_qualifying_hashes(&Tree::is_childless);
        std::sample(
            childless_hashes.begin(),
            childless_hashes.end(),
            std::inserter(p_hashes, p_hashes.begin()),
            p_remainder,
            std::mt19937{std::random_device{}()}
        );
    }
    return p_hashes;
}

bool Tree::is_childless(std::string to_check) {
    return (get_chain()[to_check].children.empty());
}

bool Tree::is_orphan(std::string to_check) {
    return (get_chain()[to_check].parents.empty());
}

bool Tree::is_intraserver_childless(std::string to_check) {
    linked<block> tcl_block = get_chain()[to_check];
    std::string server_trip = tcl_block.ref.s_trip;
    for (const auto& child : tcl_block.children) {
        if (child->ref.s_trip == server_trip) return false;
    }
    return true;
}

bool Tree::is_intraserver_orphan(std::string to_check) {
    linked<block> tcl_block = get_chain()[to_check];
    std::string server_trip = tcl_block.ref.s_trip;
    for (const auto& parent : tcl_block.parents) {
        if (parent->ref.s_trip == server_trip) return false;
    }
    return true;
}


std::unordered_set<std::string> Tree::intraserver_c_hashes(std::string to_check) {
    linked<block> tcl_block = get_chain()[to_check];
    std::unordered_set<std::string> result;
    std::string server_trip = tcl_block.ref.s_trip;
    for (const auto& child : tcl_block.children) {
        if (child->ref.s_trip == server_trip) result.insert(child->trip);
    }
    return result;
}

std::unordered_set<std::string> Tree::intraserver_p_hashes(std::string to_check) {
    linked<block> tcl_block = get_chain()[to_check];
    std::unordered_set<std::string> result;
    std::string server_trip = tcl_block.ref.s_trip;
    for (const auto& parent : tcl_block.parents) {
        if (parent->ref.s_trip == server_trip) result.insert(parent->trip);
    }
    return result;
}

std::unordered_set<std::string> Tree::get_qualifying_hashes(std::function<bool(Tree*, std::string)> qual_func, std::string s_trip) {
    std::unordered_set<std::string> qualifying_hashes;
    for (const auto& [hash, block] : get_chain()) {
        if (!s_trip.empty() && block.ref.s_trip != s_trip) continue;
        if (qual_func(this, hash)) qualifying_hashes.insert(hash);
    }
    return qualifying_hashes;
}

std::unordered_set<std::string> Tree::get_parent_hash_union(std::unordered_set<std::string> c_hashes) {
    std::unordered_set<std::string> p_hash_union;
    for (const auto& ch: c_hashes) {
        for (const auto& parent : get_chain()[ch].parents) {
            p_hash_union.insert(parent->trip);
        }
    }
    return p_hash_union;
}

std::vector<std::string> order_hashes(std::unordered_set<std::string> input_hashes) {
    std::vector<std::string> hashes_vector;
    std::copy(input_hashes.begin(), input_hashes.end(), std::back_inserter(hashes_vector));
    std::sort(hashes_vector.begin(), hashes_vector.end(), std::greater<std::string>());
    return hashes_vector;
}
