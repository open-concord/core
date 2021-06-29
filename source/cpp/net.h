#include <netinet/in.h> 

#pragma once

class Server {
    private:
        int portaddr; // port address of server instance        
        int queuelim; // prospective limits on connection queue
        int family = AF_INET; // default connection family is INET
        struct sockaddr_in addr; // socket obj
        int sockfd; // socket

        // socket initation
        void initiate_socket();

    public:
        Server(int portaddr, int queuelim, bool local);

        void open_listening();

        // Client Connection
        class Client;

        void handle_next();
        void shutdown();
};