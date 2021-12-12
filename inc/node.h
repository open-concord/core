#include <nlohmann/json.hpp>
#include "uttu.hpp"

#include "tree.h"

#include <iostream>
#include <thread>
#include <pthread.h>
#include <memory>
#include <string>
#include <map>
#include <unordered_set>
#include <chrono>
#include <vector>

using json = nlohmann::json;

struct context {
    std::vector<block> new_blocks;
    std::string chain_trip;
    std::unordered_set<std::string> seen_hashes;
    std::unordered_set<std::string> last_layer;
    size_t k, pow_min;
    bool first_layer = true;
};

struct Conn {
  std::string msg_buffer;
  std::map<std::string, Tree>* parent_chains;
  std::shared_ptr<Peer> net;
  int timeout;
  std::function<std::string(Conn*)> logic;

  context conn_context;

  Conn(
    std::map<std::string, Tree>* pm,
    std::shared_ptr<Peer> net,
    std::function<std::string(Conn*)>
  );
  void Handle();
  void Stop(); /** order 66 */
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
    /** active Conn */
    std::vector<std::shared_ptr<Conn>> alive;
    /** timeout for last communication in seconds */
    std::chrono::seconds timeout;
    /** known hosts */ // (maybe outside the scope of core?)
    std::vector<khost> known_hosts;
  public:
    std::map<std::string /*trip*/, Tree /*chain model*/> chains;

    Node(
      int queue,
      unsigned short int port,
      std::map<std::string, Tree> cm,
      int timeout,
      std::function<bool(std::string)> /** watchdog on incoming IP */
    );

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
    std::shared_ptr<Conn> Contact(std::string initial_content, std::string ip, int port);
};

std::string message_logic(Conn* conn);