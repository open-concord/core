#include <netinet/in.h> 
#include <string>

#pragma once

class Server {
    private:
        friend class Client;
        int portaddr; // port address of server instance        
        int queuelim; // prospective limits on connection queue
        int family = AF_INET; // default connection family is INET
        struct sockaddr_in addr; // socket obj
        int sockfd; // socket

    public:
        Server(int portaddr, int queuelim, bool local);
        void open_listening();
        void shutdown();
};

class Client {
    private:
    // pointer to parent hub
    Server* hub;
    // setting 'important' vars
    char buffer[256];
    int connection;
    public:
    // each client object represents a connection to the hub from a user
    // each client is created from the parent hub's connection queue
    // basically hoisting the parent hub each time a new client is made
    Client (Server* h);
    public:
    void handle(void (* handleFunctionPointer)(std::string));
    void send_connection(std::string msg);
    void close_connection(std::string reciept);
};