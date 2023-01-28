#include "../../inc/graph.hpp"

template<class vertex>
graph_model<vertex>::graph_model() {}

template<class vertex>
linked<vertex> 
graph_model<vertex>::get_root() {
  return *(this->graph_root);
}

template<class vertex>
bool 
graph_model<vertex>::check_rooted() {
  return this->rooted;
}

template<class vertex>
std::map<std::string, linked<vertex>>
graph_model<vertex>::get_graph() {
  return this->graph;
}

template<class vertex>
std::unordered_set<vertex> 
graph_model<vertex>::get_connected(std::unordered_set<vertex> to_check) {
  std::map<std::string, std::unordered_set<std::string>> parents_ref;
  std::unordered_set<std::string> conn_trips;
  std::unordered_set<vertex> conn_vertices;
  std::function<bool(std::string)> is_supported;

  for (const auto tc_vert : to_check) parents_ref[tc_vert.trip()] = tc_vert.p_trips();

  is_supported = [this, parents_ref, &conn_trips, &is_supported](std::string target) {
    for (const auto p_trip : parents_ref.at(target)) {
      if (
          conn_trips.contains(p_trip)
          || parents_ref.contains(p_trip) && is_supported(p_trip)
          || (this->graph).contains(p_trip)
          ) continue;
          return false;
    }
    conn_trips.insert(target);
    return true;
  };

  for (const auto tc_vert : to_check) {
    if (
        conn_trips.contains(tc_vert.trip()) 
        || is_supported(tc_vert.trip())
        ) conn_vertices.insert(tc_vert);
  }

  return conn_vertices;
}
