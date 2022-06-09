#pragma once
#include <nlohmann/json.hpp>
#include <uttu.hpp>

#include "tree.hpp"
#include "proto.hpp"

struct ConnCtx {  
  Peer Networking;
  
  struct {
    bool close = false;
    std::vector<block> NewBlocks;
    std::string ChainTrip;
    /** @deprecated */
    std::vector<std::string> MessageCtx; 
    // ^ yikes pt.2 (this should be taken care of by any ConnContext logic)
  } ExchangeCtx;

  struct {
    /** this should be be POLP, or at the very least a (hopefully) faster concurrent hashmap r/w */
    std::map<std::string, Tree>* ParentMap;
    /** filter ~ essentially blacklist some trees or all non-whitelisted trees */
    bool filter, blacklist = false;
    std::vector<std::string> filtered_trees; // only used if filter is true
  } GraphCtx;
  
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
  std::map<std::string, Tree> Chains;
  std::vector<ConnCtx> Connections;
  /** it's nessecary to retain a relay, just as a reliable end point for incoming connections */
  Relay Dispatcher; 
    
  bool Lazy_Active = false;
  void _Await_Stop(unsigned int t);
  std::function<void(ConnCtx*)> HookCall;
  /** a node can still function without opening itself completely */
  void Open();
  
  void Hook(std::function<void(ConnCtx*)>); // QoL, called on new Connection spawn

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
