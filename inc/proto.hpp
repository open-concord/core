#pragma once
#include <functional>
#include <vector>
#include <memory>


/** In order of authority (rn), (Node/ConnCtx) | Protocol | Whatever */

/* lowkey anti pattern */
class Node;
struct ConnCtx;

struct Protocol{
  public:
    virtual void NodeHandle(Node*) = 0;
    virtual void ConnHandle(ConnCtx*) = 0;
    virtual ~Protocol() = default;
};

