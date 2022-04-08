#include "../../inc/node_is_bloat.hpp"

/** needed for foward declaration, but be careful */
ConnContext::ConnContext() {
}

ConnContext(std::function<void(ConnContext*)> l) : Logic(l) {
};

/** @Class Node */

void Node::Open() {
  this->Dispatcher.Open();
}

void Node::Connect(std::string ip, unsigned int port) {

}

void Node::Close() {
  this->Dispatcher.Close();
}

Node::Node() {

}
