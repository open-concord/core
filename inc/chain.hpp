#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <array>
#include <compare>
#include <filesystem>
#include <queue>
#include <nlohmann/json.hpp>
#include <errno.h>
#include <atomic>
#include <mutex>
#include <cassert>

// translation model from which protocols are derived (e.g. COBS, CLAF, etc)

/*
The vertex class, a generic data structure for points on a graph, needs the following:
- vert.p_trips(), to get parent vert trips
- vert.trip(), to get the vert trip
- std::hash(vert)
- (==)(vert A, vert B)
*/

template<class vertex>
struct linked {
    std::string trip;
    vertex ref;
    std::unordered_set<linked<vertex>*> parents;
    std::unordered_set<linked<vertex>*> children;
};

template<class vertex>
class chain_model {
    public:
        chain_model();

        linked<vertex> get_root();

        bool check_rooted();

        std::map<std::string, linked<vertex>> get_chain();

        void set_push_callback(std::function<void(std::map<std::string, linked<vertex>>, std::unordered_set<std::string>)> callback);

        void queue_unit(vertex to_queue);

        void queue_batch(std::unordered_set<vertex> to_queue);

        void queue_batch(std::vector<vertex> to_queue);

        virtual void chain_configure(vertex root);
    private:
        std::map<std::string, linked<vertex>> chain;

        linked<vertex>* chain_root;

        bool rooted;

        std::queue<std::unordered_set<vertex>> awaiting_push_batches;

        std::function<void(std::map<std::string, linked<vertex>>, std::unordered_set<std::string>)> push_callback;

        std::atomic<bool> push_proc_active = false;

        std::mutex push_proc_mtx;

        void link(std::string target);

        void batch_push(std::unordered_set<vertex> to_push);

        void push_proc();

        virtual std::unordered_set<vertex> get_valid(std::unordered_set<vertex> to_check);

        virtual std::unordered_set<std::string> gen_parents(int ask = 3);
};