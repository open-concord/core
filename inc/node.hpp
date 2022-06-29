#pragma once
#include <uttu.hpp>
#include "tree.hpp"
#include "proto.hpp"

#ifndef FTEMPH
#define FTEMPH
namespace FTEMP {
  FlagManager::PRETAPE ConnCtx{5, false};
  FlagManager::PRETAPE Node{3, false};
};
#endif

namespace Ctx {
  struct Exchange {
    bool close = false;
    std::vector<block> NewBlocks;
    std::string ChainTrip;
    /** @deprecated */
    std::vector<std::string> MessageCtx; 
    // ^ yikes pt.2 (this should be taken care of by any ConnContext logic)
  };

  struct Graph {
    /** this should be be POLP, or at the very least a (hopefully) faster concurrent hashmap r/w */
    std::map<std::string, Tree>* ParentMap;
    /** filter ~ essentially blacklist some trees or all non-whitelisted trees */
    bool filter, blacklist = false;
    std::vector<std::string> filtered_trees; // only used if filter is true
  };
};


struct ConnCtx {  
  enum {
    ACTIVE,
    HALTED,
    COMPLETE,
    CLOSE,
    HOST
  } FLAGS;
  FlagManager Flags; 
  Peer Networking;
  
  Ctx::Exchange* ExchangeCtx;
  Ctx::Graph* GraphCtx;
  /** foward declare support */
  ConnCtx(); 
  void UpdateParentMap(std::map<std::string, Tree>*);
  void UpdateTimeout(unsigned int);

  /** perferable */
  ConnCtx( 
      std::map<std::string, Tree>*,
      Peer p = Peer(std::nullopt)
  );
};

class Node {
public:
  enum {
    OPEN, 
    CLOSE,
    LAZY_ACTIVE
  } FLAGS;
  FlagManager Flags;  

  std::map<std::string, Tree> Chains;
  std::vector<ConnCtx> Connections;
  /** it's nessecary to retain a relay, just as a reliable end point for incoming connections */
  Relay Dispatcher; 
  
  void _Await_Stop(unsigned int t);
  /** a node can still function without opening itself completely */
  void Open(); 

  void Lazy(bool state, bool blocking);

  void Contact(std::string ip, unsigned int port);
 
  void Stop(); // ... self & connections

  void Close(); // ... acceptor

  Node(
      unsigned short int,
      std::map<std::string, Tree>&,
      std::function<bool(std::string)>,
      unsigned short int = 10,
      unsigned int = 3000
  );
};
