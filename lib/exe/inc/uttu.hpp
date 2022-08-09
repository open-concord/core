/** ps & qs */
#pragma once
/** sys api */
#ifdef __APPLE__ // OSx

#endif
#ifdef __linux__ // linux
#include <sys/socket.h> // duh
#include <sys/types.h> // needed by socket.h
#include <netinet/in.h> // internet socket protocol
#include <strings.h> // bzero
#include <unistd.h> // close, accept, etc
#include <arpa/inet.h> // needed for ip ID
#include <netdb.h> // gethostbyname
#include <poll.h> // ppoll & poll
#endif
#ifdef _WIN64 // windows 64
#include <ws2def.h>
#endif

/** standard libs */
#include <vector>
#include <optional>
#include <thread>
#include <functional>
#include <memory>
#include <iostream>
#include <cstring>
#include <sstream>
#include <string>

/** internal */
#include "dhm.hpp"
#include "protocols.hpp"

struct FlagManager {
public:
  /** QoL */
  using PRETAPE = std::pair<unsigned int, bool>;
  using TAPE = std::vector<bool>;
/** flags */
protected:
  std::vector<std::vector<bool>> ftape;
public:
  unsigned int FlagCount(unsigned int);
  void SetFlag(unsigned int, bool, unsigned int = 0);
  bool GetFlag(unsigned int, unsigned int = 0);
  FlagManager(unsigned int); // count of tapes only
  FlagManager(FlagManager::PRETAPE); // solo
  FlagManager(std::vector<FlagManager::PRETAPE>); // blanket 
  FlagManager(std::vector<FlagManager::TAPE>); // literal 
};

#ifndef UFTEMPH
#define UFTEMPH
namespace UFTEMP {
  inline FlagManager::PRETAPE Peer {4, false};
  inline FlagManager::PRETAPE Relay {3, false}; 
};
#endif

struct Peer {
public:
    enum {
      HALTED,
      CLOSE,
      UNTRUSTED,
      HOST
    } FLAGS;
    FlagManager Flags;

    dhms sec;
    np* net;
    unsigned int tout; 
		/** Raw operations */
		std::string Raw_Read(unsigned int t = 0);
		void Raw_Write(std::string m, unsigned int t = 0);
    void Port(unsigned int);
    /** getters */
    bool Host(); // get
		/** operations */
		std::string Read(unsigned int t = 0);
		void Write(std::string m, unsigned int t = 0);
		/** state */
		void Close(); // close current socket
		void Connect(std::string ip, unsigned short int port); // change socket target	
		Peer(
        std::optional<np*> _net,
				unsigned int timeout = 3000
		    );
};

struct Relay : public Peer {
  public: 
    enum {
      LAZY,
      OPEN,
      CLOSE
    } FLAGS; 
    FlagManager Flags;
  private:
    /** config */
		unsigned short queueL;	

		std::function<bool(std::string)> _c = nullptr; // criteria
    std::function<void(Peer)> _e = nullptr; // embedded logic (for Peer foward)
		void _Lazy(unsigned int life);
    void Foward();
	public:
    void Embed(std::function<void(Peer)> p);
	  void Criteria(std::function<bool(std::string)> c);
		void Lazy(bool blocking, unsigned int life = -1);
    void Open();
		Relay(
        std::optional<np*> _net,
				unsigned short int r_port,
				unsigned int timeout,
				unsigned short _queul
        );
};

struct Timeout {
	private:
		int sk; // socket to kill on timeout
		std::jthread expire;
		std::atomic<bool> _cancel; // cancel flag
		void _async(int ft); // inner async loop
	public:
		Timeout(unsigned int t, int s);
		void Cancel();
};

void errc(std::string); // configurable error handler
void Kill_Socket(int s); // called on timeout; configurable
