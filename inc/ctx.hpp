#pragma once
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <deque>
#include <unordered_set>

#include "tree.hpp"
#include <uttu.hpp>

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

struct Conn {
  enum {
    ACTIVE,
    HALTED,
    COMPLETE,
    CLOSE
  } FLAGS;
  FlagManager Flags;
  std::unique_ptr<Peer> p;
  
  Ctx::Exchange ExchangeCtx;
  Ctx::Graph GraphCtx;
  Peer* P() {return this->p.get();} 
  Conn (
      Ctx::Graph g,
      std::unique_ptr<Peer> p
      ) : p(std::move(p)), GraphCtx(g) {
    /** reserve tape */
    this->Flags.Reserve(0, 5);
    this->Flags.Fill(false);
  }
};

struct Node {
  std::unique_ptr<Relay> r;

  enum {
    OPEN, 
    CLOSE
  } FLAGS;
  FlagManager Flags; 
  std::deque<std::unique_ptr<Conn>> Connections;  
  
  Ctx::Graph Graph;
 
  Relay* R() {return this->r.get();}
  void Lazy(bool state, bool blocking);
  void Contact(std::string ip, unsigned int port);
  void Stop();
  Node(
      unsigned short int port,
      unsigned short queueLimit = 15,
      unsigned int timeout = 3000,
      std::optional<np*> n = std::nullopt
      ) : 
    r(
        std::make_unique<Relay>( 
            n, port, timeout, queueLimit
        )
    ) {
    /** reserve tape */
    this->Flags.Reserve(0, 2);
    this->Flags.Fill(false);
    this->R()->Embed([this](std::unique_ptr<Peer> p) -> void {
        this->Connections.push_back(std::make_unique<Conn>(
          this->Graph,
          std::move(p)
        ));
      });
  }
};
