#pragma once
#include <functional>
#include <vector>
#include <memory>

/* lowkey anti pattern */
class Node;
struct ConnCtx;

struct Protocol {
  public:
    /** 
     * anything beyond the node and conn pointers is the developer's repsonibility; don't fuck up
     */
    virtual void ConnH(ConnCtx*, ...) = 0;
    virtual void NodeH(Node*, ...) = 0;
    virtual ~Protocol() = default;
};

struct Controller {
  std::vector<std::shared_ptr<Node>> NodeVec;
  std::vector<std::shared_ptr<ConnCtx>> ConnVec;

  std::vector<Protocol> Protos;
  public:
    // there needs to be some type of ledger so the index of both the nodes and vectors are set
    void LoadProto(Protocol&);
    void SetLogic(int, int, bool);
};
