#include "../../inc/graph.hpp"

template<class vertex> 
void 
graph_model<vertex>::batch_push(
    std::unordered_set<vertex> to_push_set, 
    std::unordered_set<std::string> flags // FIXME this should be a bitmask, or even a set of ints, strings are kind of wasteful here
  ) {
  std::unordered_set<vertex> valid_vertices = get_valid(to_push_set);
  std::unordered_set<vertex> usable_vertices = get_connected(valid_vertices);
  std::unordered_set<std::string> new_trips;

  // if there's a new root, we deal with it first
  // we can add and link it later - the graph just needs to be configured before the full push.
  for (const auto tp_vert : usable_vertices)
    if (tp_vert.p_hashes().empty()) graph_configure(get_graph()[tp_vert].ref);

  // add all verts, *then* link, and *only then* trigger callbacks (once verts are integrated)
  for (const auto tp_vert : usable_vertices) {
    linked<vertex> new_vert;
    new_vert.ref = tp_vert;
    new_vert.trip = tp_vert.trip();

    (this->graph)[tp_vert.trip()] = new_vert;
    new_trips.insert(tp_vert.trip());
  }

  for (const auto tp_vert : usable_vertices) link(tp_vert.trip());

  push_response(new_trips, flags);
}

// queuing (ensure that pushes don't happen simultaneously)

template<class vertex>
void 
graph_model<vertex>::queue_batch(std::unordered_set<vertex> to_queue) {
  (this->awaiting_push_batches).push(to_queue);
  (this->push_proc_mtx).lock();
  if (!push_proc_active) {
    push_proc_active = true;
    (this->push_proc_mtx).unlock();
    push_proc();
  } else (this->push_proc_mtx).unlock();
}

// Convienece overloads + methods
template<class vertex>
void 
graph_model<vertex>::queue_batch(std::vector<vertex> to_queue) {
  queue_batch(
      std::unordered_set<vertex>(
        to_queue.begin(), 
        to_queue.end(), 
        to_queue.size()
        )
      );
}

template<class vertex>
void 
graph_model<vertex>::queue_unit(vertex to_queue) {
    std::unordered_set<vertex> unit_batch;
    unit_batch.insert(to_queue);
    queue_batch(unit_batch);
}
// END Convience overloads + methods

template<class vertex>
void 
graph_model<vertex>::push_proc() {
  while (true) {
    std::unordered_set<vertex> next_batch;

    std::lock_guard<std::mutex> lk(this->push_proc_mtx);

    if ((this->awaiting_push_batches).empty()) {
      this->push_proc_active = false;
      return;
    }

    next_batch = awaiting_push_batches.front();
    (this->awaiting_push_batches).pop();

    batch_push(next_batch);
  }
}

// linking (finding parents/children)

template<class vertex>
void 
graph_model<vertex>::link(std::string to_link) {
  // unfortunately, it turns out we can't link verts that *aren't in the graph*
  if (!get_graph().contains(to_link)) return;

  linked<vertex> tl_vertex = get_graph()[to_link];

  // add parents by tripcodes, and give those parents the target as a child.
  for (const auto p_trip : tl_vertex.p_trips()) {
    (this->graph)[to_link].parents.insert(&((this->graph)[p_trip]))
    (this->graph)[p_trip].children.insert(&((this->graph)[to_link]));
  }

  // set up root references
  if (tl_vertex.ref.p_hashes().empty() && !this->rooted) {
    this->rooted = true;
    this->graph_root = &((this->graph)[to_link]);
  }
}


