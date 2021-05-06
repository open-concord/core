#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream> // For cout
#include <unistd.h> // For read


// error handling is for losers
class Server {
    private:
        int portaddr = 1337; // setting this up to be changeable programmatically        
        int sockfd;
        int queuelim = 20; // set to 20 connections rn
        struct sockaddr_in serv_addr, cli_addr;
    public:
        int create_socket() {
            sockfd = socket (AF_INET, SOCK_STREAM, 0);
            if (sockfd == -1) {
                std::cout << "Error while creating socket: " << errno << std::endl;
                // exit handling here
            } else {
                return sockfd;
            }
        }

        void start_listen() {
            sockaddr_in sockaddr;
            sockaddr.sin_family = AF_INET;
            sockaddr.sin_addr.s_addr = INADDR_ANY;
            sockaddr.sin_port = htons(this -> portaddr);

            // testing port
            if (bind(this -> sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
                std::cout << "Error while binding to port: " << errno << std::endl;
                // exit handling here
            }

            // hold queue
            if (listen(this -> sockfd, this -> queuelim) < 0) {
                std::cout << "Error on listen: " << errno << std::endl;
                // exit handling here
            }
        }

        void handle() {
            // testing with one connection rn, will add actual handling later
            auto addrlen = sizeof(sockaddr);
            int connection = accept(this -> sockfd, (struct sockaddr *) &sockaddr, &addrlen);
            if (connection < 0) {
                std::cout << "Error while grabbing connection: " << errno << std::endl;
                // exit handling here
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
            close(this -> sockfd);
        }
};