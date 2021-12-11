#include "../../inc/node.h"

/** init */
void Conn::Conn(
  std::map<std::string, Tree>* pm,
  std::shared_ptr<Peer> net,
  std::string(*logic)(*Conn)
) : parent_chains(pm), net(net), logic(logic) {
  this->handle(this->logic(&this));
}

/** recursive handling */
void Conn::Handle() {
  if (this->msg_buffer.empty()) {
    this->msg_buffer = this->Net.Read();
  } else {
    this->Net.Send(this->logic(&this));
  }
  this->Handle();
}

/** immediately cease contact */
void Conn::Stop() {
  json s = {
    {"FLAG", "ERROR"},
    /** using 0 for now (eg. no error), please update w/ CLAF legal*/
    {"CONTENT", 0}
  };
  /** send claf compliant stop */
  this->Net.Send(s.dump());
  this->Net.Close();
}
