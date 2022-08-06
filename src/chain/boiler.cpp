#include "../../inc/chain.hpp"

//this file is just boilerplate

template<class vertex>
chain_model<vertex>::chain_model() {
    return;
}

template<class vertex>
linked<vertex> chain_model<vertex>::get_root() {
    return *(this->chain_root);
}

template<class vertex>
bool chain_model<vertex>::check_rooted() {
    return this->rooted;
}

template<class vertex>
std::map<std::string, linked<vertex>> chain_model<vertex>::get_chain() {
    return this->chain;
}

template<class vertex>
void chain_model<vertex>::set_push_callback(std::function<void(std::map<std::string, linked<vertex>>, std::unordered_set<std::string>)> callback) {
    this->push_callback = callback;
}