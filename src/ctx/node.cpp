#include "../../inc/ctx.hpp"

void Node::Lazy(bool state, bool blocking) {
  this->Flags.SetFlag(Relay::LAZY, state, 1);
  if (state) {
    this->Relay::Lazy(blocking);
  }
}

void Node::Contact(std::string ip, unsigned int port) {
  Peer p(std::nullopt);
  p.Connect(ip, port);
  Conn c(
      this->Graph, 
      std::move(p) 
  );
  this->Connections.push_back(std::move(c)); 
}

void Node::Stop() {
  this->Close();
  unsigned int ht; // highest timeout
  for (auto& c: this->Connections) {
    ht = (ht < c.tout) ? c.tout : ht;
    c.Flags.SetFlag(Conn::CLOSE, true, 1); // second tape
  }
  std::jthread st([] (Node* n, unsigned int t) -> void {
    try {
      while (!n->Connections.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(t));
      }
    } catch (...) {
      // TODO lmao
      return;
    }
  }, this, ht);
  st.detach();
}
