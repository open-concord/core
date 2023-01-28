/**
 * \addtogroup Core
 * \{
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <compare>
#include <queue>
#include <atomic>
#include <mutex>
#include <cassert>
#include <functional>

/** 
 * \brief A generic data structure for points on a graph
 * 
 * Purely an abstract in this context, also requires std::size_t - std::hash(vertex).
 */
struct vertex {
  virtual std::unordered_set<std::string> p_trips() = 0; /**< Retrieve parent verticies' trips */
  virtual std::string trip() = 0; /**< Retrieve vertex's trip */
  virtual bool operator == (const vertex& lhs) = 0; /**< Equivalence of hashes */
};

/**
 * \brief A linked wrapper for vertices
 */
template<class vertex>
struct linked {
  std::string trip; /**< The vertex's trip */
  vertex ref; /**< The vertex itself */ 
  std::unordered_set<linked<vertex>*> parents; /**< Parent points */
  std::unordered_set<linked<vertex>*> children; /**< Child points */
};

/**
 * \brief Translation model from which interpretive protocols are derived
 */
template<class vertex>
class graph_model {
public:
  graph_model();
  /** \brief Get graph's root
   * \returns Graph's root
   */
  linked<vertex> get_root();
  
  /**
   * \brief Check if the graph is rooted
   * \returns Truth statement
   */
  bool check_rooted();
  
  /**
   * \brief Retrieve the graph
   * \return Graph model's graph
   */
  std::map<std::string, linked<vertex>> get_graph();

  // FIXME This should either be removed, defined, or set as a virtual. I'm leaning towards defined :^)
  void set_push_callback(
    std::function<void(
      std::map<
        std::string,
        linked<vertex>>, 
      std::unordered_set<std::string>
    )> callback);

  /**
   * \brief Push a singular vertex to queue
   * \param to_queue Vertex to queue
   */
  void queue_unit(vertex to_queue);
  
  /** 
   * \brief Pushes batches to the queue
   * \param to_queue Batch to queue
   */
  void queue_batch(std::unordered_set<vertex> to_queue);
  
  /**
   * \overload
   */
  void queue_batch(std::vector<vertex> to_queue);
protected:
  std::map<std::string, linked<vertex>> graph; /**< Graph */
  linked<vertex>* graph_root; /**< Graph root */
  bool rooted; /**< Truth state of graph root */
  
  std::queue<std::unordered_set<vertex>> awaiting_push_batches; /**< Queued batches */
  std::atomic<bool> push_proc_active = false; /**< Truth state of push proc */
  std::mutex push_proc_mtx; /**< Memlock of push proc */

  /**
   * \brief Link vertex to graph as linked<vertex>
   * \param target Hash of vertex
   */
  void link(std::string target);
 
  /** 
   * \brief Push a batch of verticies to the graph
   * \param to_push_set Set of verticies to push
   * \param flags Set of flags to pass on to callbacks
   *
   * Pushes a batch to the graph, and then calls each respective server's callback with relevant hashes
   */
  void batch_push(std::unordered_set<vertex> to_push, std::unordered_set<std::string> flags = std::unordered_set<std::string>());
  
  /**
   * \brief Apply queued verticies
   * 
   * Blocking
   */
  void push_proc();
 
  /**
   * \brief Retrieve all verticies which are connected to a given set of verticies to a depth of +/- 1
   * \param to_check Set of verticies to check
   * \returns All connected vertices (depth +/- 1)
   */
  std::unordered_set<vertex> get_connected(std::unordered_set<vertex> to_check);

  virtual void graph_configure(vertex root) = 0;
  virtual std::unordered_set<vertex> get_valid(std::unordered_set<vertex> to_check) = 0;

  virtual void push_response(
      std::unordered_set<std::string> new_trips,
      std::unordered_set<std::string> flags = std::unordered_set<std::string>()
      ) = 0;
};

/** \} */
