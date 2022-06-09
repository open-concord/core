#pragma once
#include <proto.hpp>
#include <uttu/uttu.hpp>

struct ui : public Protocol {
private:
  Node* n;
  ConnCtx* c;
  
  Protocol::ConnHandle ch;
  Protocol::NodeHandle nh;

  /** ui uses CSP */
  Peer* p;

public:
  void NodeHandle(Node*) override;
  void ConnHandle(ConnCtx*) override;

  /** we need an actual protocol to use */
  ui(Protocol::ConnHandle& c_, Protocol::NodeHandle& n_) : ch(c_), nh(n_) {     
  }
};
