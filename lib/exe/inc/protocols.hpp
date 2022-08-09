#include <string>

/** extendable formatting */
struct np {
protected:
  struct sockaddr_in self = {};
  int cfamily = AF_INET;
  int sockfd = -1;
public:
  struct _tf {
    std::string addr;
    unsigned short int port;
  } tf; // target format

  /** form socket */ 
  virtual struct sockaddr_in _form(int port = -1) = 0;

  /** origin */
  virtual void target(_tf t) = 0;
  virtual void port(unsigned short int _port) = 0;
  virtual void queue(int origin_fd) = 0;

  /** operations */
  virtual std::string readb() = 0;
  virtual void writeb(std::string m) = 0;
  virtual void closeb() = 0;

  /** status */
  virtual int socketfd() = 0;
  virtual std::string peer_ip() = 0;

  virtual ~np() = default;
};
