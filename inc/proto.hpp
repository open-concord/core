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
<<<<<<< HEAD
    virtual void NodeHandle(Node*) = 0;
    virtual void ConnHandle(ConnCtx*) = 0;
||||||| d1bc760
    /** 
     * anything beyond the node and conn pointers is the developer's repsonibility; don't fuck up
     */
    virtual void ConnH(ConnCtx*, ...) = 0;
    virtual void NodeH(Node*, ...) = 0;
=======
    /** 
     * anything beyond the node and conn pointers is the developer's repsonibility; don't fuck up
     */
    virtual void ConnH(ConnCtx*) = 0;
    virtual void NodeH(Node*) = 0;
>>>>>>> 26d62ebbefd9871606367f562385e5ac8ef314f6
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
