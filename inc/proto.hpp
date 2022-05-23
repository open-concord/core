#pragma once
#include <functional>
#include <vector>
#include <memory>


/** In order of authority (rn), (Node/ConnCtx) | Controller | Whatever */

/* lowkey anti pattern */
class Node;
struct ConnCtx;

struct Protocol{
  public:
    virtual void NodeHandle(Node*) = 0;
    virtual void ConnHandle(ConnCtx*) = 0;
    virtual ~Protocol() = default;
};

/** essentially a temp solution to an eventually (hopefully) obsolete system. */

struct Controller {
  /** ffs */
  /** actually vomit inducing, but at this point i couldn't care less
   * if anything, it's motivation to get the other one done
   * ~u2on
   */

  std::map<Protocol*, std::tuple<Node*, ConnCtx*>> ControlM;

  public: 
    void Load(Protocol* p, Node* n = nullptr, ConnCtx* c = nullptr) { 
      ControlM[p] = std::make_tuple(n, c);
    }
};
