#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream> // For cout
#include <unistd.h> // For read
#include <string>

/**
 * Initiation of a Server instance
 * Defualt version is INET aka Internet
 * Option to switch to LOCAL is taken as init argument
 * 
 * In either case, it's nessecary to provide a port number
 */
class Server {
    private:
        int portaddr; // port address of server instance        
        int queuelim; // prospective limits on connection queue
        int family = AF_INET; // default connection family is INET
        int sockaddr; // socket obj
        int sockfd; // socket

        // socket initation
        void intiate_socket() {
            struct sockaddr_in serv_addr, cli_addr;
            sockaddr_in sockaddr;

            sockaddr.sin_family = this->family;
            sockaddr.sin_addr.s_addr = INADDR_ANY;
            sockaddr.sin_port = htons(this->portaddr);

            sockfd = socket (this->family, SOCK_STREAM, 0);

            if (this->sockfd < 0) {
                std::cout << "Error while creating socket: " << errno << std::endl;
                // exit handling here
                std::cout << EXIT_FAILURE << std::endl;
                exit(EXIT_FAILURE);
            }
        }

    public:
        Server(int portaddr, int queuelim, bool local) {
            this->portaddr = portaddr;
            this->queuelim = queuelim;
            if (local) {this->family = AF_LOCAL;}
            this->intiate_socket();
        };

        void open_listening() {
            // binding port
            if (bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
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
        class Client {
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
                Client (Server * h) : hub(h);

                // overwrite this function to take over input handling
                void handle();

                void close_connection(std::string receipt);


        };
        void handle_next() {
            // handle new block
            Client new_connection(this);
            new_connection.handle();
        };
        void shutdown() {
            close(sockfd);
        };
};