#include "../../inc/ctx.hpp"

void Node::Lazy(bool state, bool blocking) {
  (this->r.get())->Flags.Set(Relay::LAZY, state, 1);
  if (state) {
    (this->r.get())->Lazy(blocking);
  }
}

void Node::Contact(std::string ip, unsigned int port) {
  this->Connections.push_back(
    std::make_unique<Conn>(     
      this->Graph,
      std::make_unique<Peer>(
        std::nullopt
      ) 
    )
  ); 
  this->Connections.back()->P()->Connect(ip, port);
}

void Node::Stop() {
  (this->r.get())->Close();
  unsigned int ht; // highest timeout
  for (auto& c: this->Connections) {
    auto cv = c.get();
    ht = (ht < (cv->p.get())->tout) ? (cv->p.get())->tout : ht;
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
