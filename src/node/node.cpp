// std
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>
#include <map>


#include "../../inc/node.hpp"
#include "../../inc/tree.hpp"

Node::Node(
  unsigned short int port,
  std::map<std::string, Tree>& cm,
  std::function<bool(std::string)> wd,
  unsigned short int queue,
  unsigned int tout
) : Flags(FTEMP::Node), Chains(cm), 
  Dispatcher(
    Relay(
      std::nullopt, 
      port, 
      tout, 
      queue
    )
  )
{ 
  this->Dispatcher.Criteria(wd);
  Flags.SetFlag(Node::LAZY_ACTIVE, false);
}

void Node::Lazy(bool state, bool blocking) {
  Flags.SetFlag(Node::LAZY_ACTIVE, state);
  this->Dispatcher.Swap(([this] (Peer* np) {
    if (this->Flags.GetFlag(Node::LAZY_ACTIVE)) {
      ConnCtx c(
        &(this->Chains),
        *np
      );
      this->Connections.push_back(std::move(c));
    } else {return;}
  }));
  this->Dispatcher.Lazy(blocking);
}

void Node::Open() {
  this->Dispatcher.Open();
}

void Node::Close() {
  this->Dispatcher.Close();
}

void Node::_Await_Stop(unsigned int t) {
  try {
    while (!this->Connections.empty()) {
      // t => ms
      std::this_thread::sleep_for(std::chrono::milliseconds(t));
    }
  } catch (...) {
    /** the Node was killed :( */
    return;
  }
  exit(0); // kill node
}

void Node::Stop() {
  this->Close();
  unsigned int ht; // highest timeout
  for (auto& c: this->Connections) {
    ht = (ht < c.Networking.tout) ? c.Networking.tout : ht;
    c.Flags.SetFlag(ConnCtx::CLOSE, true); 
  }
  std::jthread st(&Node::_Await_Stop, this, ht);
  st.detach();
}

void Node::Contact( 
    std::string ip, 
    unsigned int port
  ) {
  /** new peer */
  Peer p(std::nullopt);
  p.Connect(ip+":"+std::to_string(port));
  ConnCtx nc(
      &this->Chains,
      std::move(p)
  );
  this->Connections.push_back(nc); 
}
