#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream> // For cout
#include <unistd.h> // For read


// error handling is for losers

class Server {
    private:
        int portaddr = 3000; // setting this up to be changeable programmatically        
        int queuelim = 20; // set to 20 connections rn

        // socket
        struct sockaddr_in serv_addr, cli_addr;
        sockaddr_in sockaddr;
        int sockfd;

        void start_socket() {
            sockaddr.sin_family = AF_INET;
            sockaddr.sin_addr.s_addr = INADDR_ANY;
            sockaddr.sin_port = htons(portaddr);
        }

    public:
        int create_socket() {
            this->start_socket();
            sockfd = socket (AF_INET, SOCK_STREAM, 0);
            if (sockfd == -1) {
                std::cout << "Error while creating socket: " << errno << std::endl;
                // exit handling here
                std::cout << EXIT_FAILURE << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        void start_listen() {
            // testing port
            if (bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
                std::cout << "Error while binding to port: " << errno << std::endl;
                // exit handling here
                exit(EXIT_FAILURE);
            }

            // hold queue
            if (listen(sockfd, queuelim) < 0) {
                std::cout << "Error on listen: " << errno << std::endl;
                // exit handling here
                exit(EXIT_FAILURE);
            }
        }

        void handle() {
            // testing with one connection rn, will add actual handling later
            auto addrlen = sizeof(sockaddr);

            // fixing accept
            int connection = accept(sockfd, (struct sockaddr *) &sockaddr, (socklen_t*)&addrlen);

            if (connection < 0) {
                std::cout << "Error while grabbing connection: " << errno << std::endl;
                // exit handling here
                exit(EXIT_FAILURE);
            }

            // read connection
            char buffer[256];
            auto bytesread = read(connection, buffer, 255);
            std::cout << "Received: " << buffer;

            // confirm receipt
            std::string resp = "We don't have status codes but we got ur message";
            send(connection, resp.c_str(), resp.size(), 0);

            // close connection
            close(connection);
        }

        void shutdown() {
            close(sockfd);
        }
};