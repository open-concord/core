#include "../../inc/node.hpp"
/** init */
/** default timeout init overload */
Conn::Conn(
  std::map<std::string, Tree>* pm,
  std::shared_ptr<Peer> net,
  std::function<std::string(Conn*)> l
) : parent_chains(pm), net(net), logic(l), timeout(3000) {
  if ((this->net.get())->Host()) {
    this->Handle();
  }
}
/** custom timeout init overload */
Conn::Conn(
  std::map<std::string, Tree>* pm,
  std::shared_ptr<Peer> net,
  std::function<std::string(Conn*)> l,
  unsigned int t
) : parent_chains(pm), net(net), logic(l), timeout(t) {
  if ((this->net.get())->Host()) {
    this->Handle();
  }
}

/** recursive handling */
void Conn::Handle() {
  if (this->msg_buffer.empty()) {
    this->msg_buffer = (this->net.get())->Read(this->timeout);
  }
  (this->net.get())->Write(this->logic(this), this->timeout);
  this->msg_buffer.clear();
  if (!this->stop) {
    this->Handle();
  }
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

void Conn::Prompt(json fc) {
  (this->net.get())->Write(fc.dump(), this->timeout);
  this->Handle();
}

void Conn::HCLC_Prompt(std::string chain_trip) {
  Prompt(client_open(this, chain_trip));
}
