#include "../../inc/ctx.hpp"

void Node::Lazy(bool state, bool blocking) {
  if (state) {
    this->R()->Lazy(blocking);
  } else {
    this->R()->Flags.Set(Relay::LAZY, state, 1);
  }
}

void Node::Contact(std::string ip, unsigned int port) {
  Peer p(std::nullopt);
  p.Connect(ip, port); 
  this->Connections.push_back(
    std::make_unique<Conn>(     
      this->Graph,
      std::make_unique<Peer>(p) 
    )
  );  
}

void Node::Stop() {
  (this->r.get())->Close();
  unsigned int ht; // highest timeout
  for (auto& c: this->Connections) {
    auto cv = c.get();
    ht = (ht < (cv->P())->tout) ? (cv->P())->tout : ht;
    cv->Flags.Set(Conn::CLOSE, true); 
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
