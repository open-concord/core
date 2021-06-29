#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream> // For cout
#include <unistd.h> // For read
#include <string>

#include "net.h"

/**
 * Initiation of a Server instance
 * Defualt version is INET aka Internet
 * Option to switch to LOCAL is taken as init argument
 * 
 * In either case, it's nessecary to provide a port number
 */
Server::Server(int portaddr, int queuelim, bool local) {
    this->portaddr = portaddr;
    this->queuelim = queuelim;
    int family = AF_INET;
    if (local) {family = AF_LOCAL;}
    
    // initate socket
    struct sockaddr_in serv_addr, cli_addr;

    addr.sin_family = family;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(portaddr);

    this->sockfd = socket (family, SOCK_STREAM, 0);

    if (this->sockfd < 0) {
        std::cout << "Error while creating socket: " << errno << std::endl;
        // exit handling here
        std::cout << EXIT_FAILURE << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::open_listening() {
    // binding port
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        std::cout << "Error while binding to port: " << errno << std::endl;
        // exit handling here
        exit(EXIT_FAILURE);
    }
            
    // checking socket listen
    if (listen(sockfd, queuelim) < 0) {
        std::cout << "Error on listen: " << errno << std::endl;
        // exit handling here
        exit(EXIT_FAILURE);
    }
};
void Server::shutdown() {
    close(sockfd);
};