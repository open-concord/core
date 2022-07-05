#pragma once
#include <vector>
#include <string>
#include <map>
#include <functional>

#include "tree.hpp"
#include "proto.hpp"
#include <uttu.hpp>

namespace Ctx {
  struct Exchange {
    bool complete = false;
    std::vector<block> NewBlocks;
    std::string ChainTrip;
  };
  struct Graph {
    std::map<std::string, Tree>* Forest;
    bool filter, blacklist = false;
    std::vector<std::string> filtered_trees;
  };
}

struct Conn : public Peer {
  enum {
    ACTIVE,
    HALTED,
    COMPLETE,
    CLOSE,
    HOST
  } FLAGS;

  Ctx::Exchange ExchangeCtx;
  Ctx::Graph GraphCtx;

  Conn(
      unsigned int p,
      Ctx::Graph g,
      std::optional<np*> pr = std::nullopt
      ) : Peer(pr, p), GraphCtx(g) {
    /** reserve tape */
    this->Flags.Reserve(1, 5);
  }
  Conn (
      Ctx::Graph g,
      Peer p
      ) : Peer(p), GraphCtx(g) {
    /** reserve tape */
    this->Flags.Reserve(1, 5);
  }
};

struct Node : public Relay {
  enum {
    OPEN, 
    CLOSE
  } FLAGS;

  std::vector<Conn> Connections;  
  
  Ctx::Graph Graph;
  
  void Lazy(bool state, bool blocking);
  void Contact(std::string ip, unsigned int port);
  void Stop();
  Node(
      unsigned short int port,
      unsigned short queueLimit = 15,
      unsigned int timeout = 3000,
      std::optional<np*> n = std::nullopt
      ) : Relay(n, port, timeout, queueLimit) {
    /** reserve tape */
    this->Flags.Reserve(2, 2); 
    this->Relay::Embed([this](Peer p) -> void {
        Conn c(
          this->Graph,
          std::move(p) 
        );
        this->Connections.push_back(std::move(c));
      });
  }
};
