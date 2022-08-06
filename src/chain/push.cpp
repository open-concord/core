#include "../../inc/chain.hpp"

template<class vertex> 
void chain_model<vertex>::batch_push(std::unordered_set<vertex> to_push_set) {
    std::unordered_set<vertex> usable_vertices = get_valid(to_push_set);
    std::unordered_set<std::string> new_trips;

    //add all blocks, *then* link, and *only then* trigger callbacks once blocks are fully added
    for (const auto tp_vert : usable_vertices) {
        linked<vertex> new_vert;
        new_vert.ref = tp_vert;
        new_vert.trip = tp_vert.trip();

        (this->chain)[tp_vert.trip()] = new_vert;
        new_trips.insert(tp_vert.trip());
    }

    for (const auto tp_vert : usable_vertices) 
        link(tp_vert.trip());

    (this->push_callback)(get_chain(), new_trips);
}

//queuing (ensure that pushes don't happen simultaneously)

template<class vertex>
void chain_model<vertex>::queue_batch(std::unordered_set<vertex> to_queue) {
    (this->awaiting_push_batches).push(to_queue);
    (this->push_proc_mtx).lock();
    if (!push_proc_active) {
        push_proc_active = true;
        (this->push_proc_mtx).unlock();
        push_proc();
    } else {
        (this->push_proc_mtx).unlock();
    }
}

template<class vertex>
void chain_model<vertex>::push_proc() {
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

//linking (finding parents/children)

template<class vertex>
void chain_model<vertex>::link(std::string to_link) {
    //unfortunately, it turns out we can't link blocks that *aren't in the chain*
    if (!get_chain().contains(to_link)) return;

    linked<vertex> tl_vertex = get_chain()[to_link];

    // add parents by tripcodes, and give those parents the target as a child.
    for (const auto p_trip : tl_vertex.p_trips()) {
        (this->chain)[to_link].parents.insert(&((this->chain)[p_trip]))
        (this->chain)[p_trip].children.insert(&((this->chain)[to_link]));
    }
    
    //update whether chain/server is rooted.
    //root ambiguity cannot be handled - if you allow multiple roots, this sets the first to be linked.
    if (get_chain()[to_link].parents.empty() && !(this->rooted)) {
        this->rooted = true;
        this->chain_root = &((this->chain)[to_link]);
    }
}

// outward-facing API extras

template<class vertex>
void chain_model<vertex>::queue_unit(vertex to_queue) {
    std::unordered_set<vertex> unit_batch;
    unit_batch.insert(to_queue);
    queue_batch(unit_batch);
}

template<class vertex>
void chain_model<vertex>::queue_batch(std::vector<vertex> to_queue) {
    queue_batch(std::unordered_set<vertex>(to_queue.begin(), to_queue.end(), to_queue.size()));
}