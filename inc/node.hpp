#include <nlohmann/json.hpp>
#include <uttu.hpp>

#include "tree.hpp"

#include <iostream>
#include <thread>
#include <pthread.h>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <unordered_set>
#include <chrono>
#include <vector>

using json = nlohmann::json;

struct exchange_context {
  std::vector<block> new_blocks;
  std::string chain_trip;
  size_t pow_min;
};

struct Conn {
  /** flags */
  int timeout;
  bool stop=false;
  /** storage */
  std::string msg_buffer;
  std::map<std::string, Tree>* parent_chains;
  struct exchange_context ctx;
  /** operators */
  std::shared_ptr<Peer> net;
  std::function<std::string(Conn*)> logic;

  /** custom timeout */
  Conn(
    std::map<std::string, Tree>* pm,
    std::shared_ptr<Peer> net,
    std::function<std::string(Conn*)> l,
    unsigned int t
  );
  /** default (3 sec) timeout */
  Conn(
    std::map<std::string, Tree>* pm,
    std::shared_ptr<Peer> net,
    std::function<std::string(Conn*)> l
  );
  void Handle();
  void Stop(); /** order 66 */
  void Prompt(json fc);
  void HCLC_Prompt(std::string chain_trip);
};

class Node {
  private:
    // past connection retention
    struct khost {
      std::string address;
      unsigned short int port;
      std::chrono::high_resolution_clock::time_point last_verified;
    };
    /** networking */
    Session sesh;
    bool Lazy_Active;
    /** active Conn */
    std::vector<std::shared_ptr<Conn>> alive;
    /** timeout for last communication in seconds */
    std::chrono::seconds timeout;
    /** known hosts */ // (maybe outside the scope of core?)
    std::vector<khost> known_hosts;
    /** handling logic **/
    std::function<std::string(Conn*)> logic;
  public:
    std::map<std::string /*trip*/, Tree /*chain model*/>& chains;

    Node(
      int queue,
      unsigned short int port,
      std::map<std::string, Tree>& cm,
      int timeout,
      std::function<std::string(Conn*)> handling_logic,
      std::function<bool(std::string)> wd, /** watchdog on incoming IP */
    );

    /** toggle lazy accept */
    void Is_Lazy(bool state, bool blocking);

    /** open acceptor */
    void Open();

    /** close acceptor */
    void Close();

    /** cleanly finish handling interactions */
    void _Await_Stop(int t);
    void Stop();

    /** passive communication (eg traditional server role) */
    void Next();

    /** active communication (eg traditional client role) */
    std::shared_ptr<Conn> Contact(std::string chain_trip, int k, std::string ip, int port);
};

/** here until logic API update */
json client_open(Conn *conn, std::string chain_trip);

std::string hclc_logic(Conn *conn);
