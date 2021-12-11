#include "../../inc/node.h"

/** init */
Conn::Conn(
  std::map<std::string, Tree>* pm,
  std::shared_ptr<Peer> net,
  std::function<std::string(Conn*)> l
) : parent_chains(pm), net(net), logic(l) {
  this->Handle();
}

/** recursive handling */
void Conn::Handle() {
  if (this->msg_buffer.empty()) {
    this->msg_buffer = (this->net.get())->Read(this->timeout);
  } else {
    (this->net.get())->Write(this->logic(this), this->timeout);
  }
  this->Handle();
}

/** immediately cease contact */
void Conn::Stop() {
  json j = {
    {"FLAG", "ERROR"},
    /** using 0 for now (eg. no error), please update w/ CLAF legal*/
    {"CONTENT", 0}
  };
  std::string s(j.dump());

  /** send claf compliant stop */
  (this->net.get())->Write(s, this->timeout);
  (this->net.get())->Close();
}
