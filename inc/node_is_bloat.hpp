#include <nlohmann/json.hpp>
#include <uttu.hpp>

#include "tree.hpp"

struct ConnContext {
  Peer Networking;
  struct {
    std::vector<block> NewBlocks;
    std::string ChainTrip;
    size_t PoW_Min; // yikes
    std::string MessageBuffer; // yikes pt.2
  } ExchangeContext;

  struct {
    /** this should be be POLP, or at the very least a (hopefully) faster concurrent hashmap r/w */
    std::map<std::string, Tree>* ParentMap;

  } GraphContext;
  
  std::function<void(ConnContext*)> Logic;
  
  ConnContext();
  ConnContext(std::function<void(ConnContext*)>);
};

class Node {
private:
  std::vector<ConnContext> Connections;
  
  std::function<void(ConnContext*)> DefaultConnectionLogic;
  
public:
  /** a node can still function without opening itself completely */
  void Open();

  void Connect(std::string ip, unsigned int port);
  
  void Close();
};
