#pragma once
#include <functional>
#include <vector>
#include <memory>


/** In order of authority (rn), (Node/ConnCtx) | Protocol | Whatever */

struct Node;
struct Conn;

struct Protocol{
  public:
    virtual void NodeHandle(Node*) = 0;
    virtual void ConnHandle(Conn*) = 0;
    virtual ~Protocol() = default; 
};

