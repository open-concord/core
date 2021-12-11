// std
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>
#include "../../inc/node.h"
#include "../../inc/tree.h"

/** fix to use normal fs */
/**
std::vector<std::string> get_directories(const std::string& s) {
    std::vector<std::string> r;
    for(auto& p : fs::recursive_directory_iterator(s))
        if (is_directory(p))
            r.push_back(p.path().string());
    return r;
}
*/

Node::Node(
  int queue,
  unsigned short int port,
  std::map<std::string, Tree> cm,
  int timeout,
  std::function<bool(std::string)> wd
) : sesh(Create(port, queue)) {
  this->sesh.Criteria(wd);
  //this->chains_dir = chains_dir;
  this->chains = cm;
}

void Node::Open() {
  this->sesh.Open();
}

void Node::Close() {
  this->sesh.Close();
}

void Node::_Await_Stop(int t) {
  try {
    while (!this->alive.empty()) {
      // t => ms
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
  } catch (...) {
    /** the Node was killed :( */
    return;
  }
  exit(0); // kill node
}

void Node::Stop() {
  this->Close();
  int ht; // highest timeout
  for (auto& c: this->alive) {
    ht = (ht < c.get()->timeout) ? c.get()->timeout : ht;
    c.get()->Stop();
  }
  std::thread st(&Node::_Await_Stop, this, ht);
  st.detach();
}

void Node::Next() {
  std::shared_ptr<Peer> np = this->sesh.Accept();
  std::shared_ptr<Conn> nc = std::make_shared<Conn>(Conn(&(this->chains), np, message_logic));
  this->alive.push_back(nc);
}

std::shared_ptr<Conn> Node::Contact(std::string msg, std::string ip, int port) {
  std::shared_ptr<Peer> np = this->sesh.Connect(ip+':'+std::to_string(port));
  std::shared_ptr<Conn> nc = std::make_shared<Conn>(Conn(&this->chains, np, message_logic));
  this->alive.push_back(nc);
  return nc;
}
