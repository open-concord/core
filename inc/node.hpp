#pragma once
#include <uttu.hpp>
#include "tree.hpp"
#include "proto.hpp"

struct FlagManager {
/** flags */
protected:
  std::vector<bool> flags;
public:
  void SetFlag(unsigned int, bool);
  bool GetFlag(unsigned int); 
  FlagManager(unsigned int);
};

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


struct ConnCtx : public FlagManager {  
  enum _FLAG_ENUM {
    ACTIVE,
    HALTED,
    COMPLETE,
    CLOSE
  } FLAGS;

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

class Node : public FlagManager {
public:
  enum _FLAG_ENUM {
    ACTIVE,
    HALTED,
    COMPLETE,
    CLOSE
  } FLAGS;
  
  std::map<std::string, Tree> Chains;
  std::vector<ConnCtx> Connections;
  /** it's nessecary to retain a relay, just as a reliable end point for incoming connections */
  Relay Dispatcher; 
    
  bool Lazy_Active = false;
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
      unsigned short int,
      unsigned int
  );
};
