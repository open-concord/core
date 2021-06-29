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
    if (local) {family = AF_LOCAL;}
    this->initiate_socket();
};

void Server::initiate_socket() {
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
};

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

// Client Connection
class Server::Client {
    private:
        // pointer to parent hub
        Server * hub;
        // setting 'important' vars
        char buffer[256];
        int connection;
    public:
        // each client object represents a connection to the hub from a user
        // each client is created from the parent hub's connection queue
        // basically hoisting the parent hub each time a new client is made
        Client (Server * h) : hub(h) {

            // pickup the next connection in queue
            auto addrlen = sizeof(hub->addr);

            // accept new connection
            this->connection = accept(hub->sockfd, (struct sockaddr *) &hub->addr, (socklen_t*)&addrlen);

            if (this->connection < 0) {
                std::cout << "Error while grabbing connection: " << errno << std::endl;
                // exit handling here
                exit(EXIT_FAILURE);
            }

            // read connection
            auto bytesread = read(connection, buffer, 255);
        }

        // overwrite this function to take over input handling
        void handle() {
            std::cout << "Received: " << buffer;
        }

        void close_connection(std::string receipt) {
            // end communication w/ receipt
            send(connection, receipt.c_str(), receipt.size(), 0);
            // close socket connection
            close(connection);
        }
};

void Server::handle_next() {
    // handle new block
    Client new_connection(this);
    new_connection.handle();
};
void Server::shutdown() {
    close(sockfd);
};