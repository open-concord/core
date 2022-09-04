#pragma once
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <deque>
#include <unordered_set>

#include "tree.hpp"
#include <peer.hpp>
#include <relay.hpp>
#include <debug.hpp>

namespace Ctx {
  struct Exchange {
    bool complete = false;
    std::unordered_set<block> NewBlocks;
    std::string ChainTrip;
    Tree* CurrentTree;
  };
  struct Graph {
    std::map<std::string, Tree*> Forest;
    bool filter, blacklist = false;
    std::deque<std::string> filtered_trees;
  };
}

struct Conn : public Peer {
  enum {
    ACTIVE,
    HALTED,
    COMPLETE,
    CLOSE
  } FLAGS;
  
  Ctx::Exchange ExchangeCtx;
  Ctx::Graph GraphCtx;
 
  Conn (
      Ctx::Graph g,
      Peer&& p
  ) : Peer(p), GraphCtx(g) {
    /** reserve tape */
    this->Flags.Reserve(1, 5);
    this->Flags.Fill(false, 1);
  }
};

struct Node : public Relay {
  enum {
    ACTIVE
  } FLAGS;
  
  std::deque<std::unique_ptr<Conn>> Connections;   
  Ctx::Graph Graph;
 
  void Lazy(bool state, bool blocking);
  void Absorb(Conn&& c);

  void Stop();
  Node(
      unsigned short int port,
      np& n,
      unsigned short queueLimit = 15,
      unsigned int timeout = 3000
    ) :
      Relay(n, port, timeout, queueLimit)
  {
    /** embed w/ conn modifier */ 
    this->Relay::Embed([this](Peer&& p) -> void {
      this->Connections.push_back(std::make_unique<Conn>(
        this->Graph,
        std::move(p)
      ));
    });
  }
};
