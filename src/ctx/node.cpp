#include "../../inc/ctx.hpp"
#include <thread>

void Node::Lazy(bool state, bool blocking) {
  if (state) {
    this->Relay::Lazy(blocking);
  } else {
    this->Flags.Set(Relay::LAZY, state, 1);
  }
}

void Node::Contact(std::string ip, unsigned int port) {
  Peer&& p = Peer(); // FAIL, while working out kinks in uttu
  p.Connect(ip, port); 
  this->Connections.push_back(
    std::make_unique<Conn>(     
      this->Graph,
      std::move(p) 
    )
  );  
}

void Node::Stop() {
  this->Relay::Close();
  unsigned int ht; // highest timeout
  for (auto& c: this->Connections) {
    auto cp = c.get();
    ht = (ht < cp->Peer::tout) ? cp->Peer::tout : ht;
    cp->Flags.Set(Conn::CLOSE, true, 1); 
  }
  std::jthread st([this, ht]() -> void {
    try {
      while (!this->Connections.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ht));
      }
    } catch (std::exception& e) {
      debug.bump("[!!] ", e.what());
      return;
    }
  });
  st.detach();
}
