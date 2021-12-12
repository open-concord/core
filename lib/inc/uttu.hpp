// sys api
#ifdef __APPLE__ // OSx

#endif
#ifdef __linux__ // linux and linux based
  #include <sys/socket.h> // duh
  #include <sys/types.h> // needed by socket.h
  #include <netinet/in.h> // internet socket protocol
  #include <strings.h> // bzero
  #include <unistd.h> // close, accept, etc
  #include <arpa/inet.h> // needed for ip ID
  #include <netdb.h> // gethostbyname
  #include <poll.h> // ppoll & poll
#endif
#ifdef _WIN64 // windows 64x32 systems
  #include <ws2def.h>
#endif

// standard libs
#include <vector>
#include <optional>
#include <thread>
#include <functional>
#include <memory>
#include <iostream>
#include <cstring>
#include <nlohmann/json.hpp>
// sharding across different sections to reduce bloat
#include "sec.hpp"

using json = nlohmann::json;

struct Peer { // connected peer
private:
  // socket/ip info
  dhms sec; // dhms
  int sockfd; // socket
  struct sockaddr_in sockaddr; // socket info
  // flags
  bool local = false; // local connect? && used in logic loop
  bool host = true;
  // raw functions
  std::string Raw_Read(unsigned int t);
  void Raw_Write(std::string m, unsigned int t);
public:
  // Utility
  int Socket(); // get
  bool Local(); // get
  bool Host(); // get
  // Runtime
  Peer(int sock, struct sockaddr_in socka, bool local, bool host);
  void Start(std::function<void(Peer*)> h); // r only, no need for shared_ptr

  std::string Read(unsigned int t);
  void Write(std::string m, unsigned int t);
  void Close();
};

struct Session { // socket session
private:
  // config
  unsigned short port;
  unsigned short queue_limit;
  // socket info
  int sockfd;
  struct sockaddr_in sockaddr;
  // flags
  bool close = false;
  // status/connection management
  std::function<bool(std::string)> _criteria; // accept criteria function, takes IP
  // inner thread loop for Lazy()
  void _Lazy(std::function<void(std::shared_ptr<Peer>)> h);
  std::thread _lt;
public:
  // Utility
  int Socket();
  void Criteria(std::function<bool(std::string)> criteria);
  Session (
    unsigned short port,
    unsigned short queue_limit,
    int sockfd,
    struct sockaddr_in _self
  );
  // Runtime
  void Open();
  std::shared_ptr<Peer> Accept(); /** incoming connections */
  std::shared_ptr<Peer> Connect(std::string ip); /** outbound connections */
  void Lazy(std::function<void(std::shared_ptr<Peer>)> h); /** lazy accept */
  void Close();
};

struct Timeout {
private:
  int sk; // socket to kill on timeout
  std::thread expire;
  std::atomic<bool> _cancel; // cancel flag
  void _async(int ft); // inner async loop
public:
  Timeout(unsigned int t, int s);
  void Cancel();
};

Session Create(
  unsigned short port,
  unsigned short queue_limit
);

void errc(std::string); // configurable error handler
void Kill_Socket(int s); // called on timeout; configurable
