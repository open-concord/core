#include <string>

#pragma once

std::string get_time();

std::string convert_to_local();

std::string calc_hash (bool use_disk, std::string target);

class Miner {
    private:
        int pow;
        std::string hash;
    public:
        Miner(int POW_req);

        // checking given hash for compliance w/ chain pow
        bool check_valid_hash(std::string hash);

        // genning random nonce
        std::string generate_nonce();

        // genning nonce satisfying pow
        std::string generate_valid_nonce(bool debug_info, std::string content);
};

class Server {
    private:
        int portaddr; // port address of server instance        
        int queuelim; // prospective limits on connection queue
        int family; // default connection family is INET
        int sockaddr; // socket obj
        int sockfd; // socket

        // socket initation
        void intiate_socket();

    public:
        Server(int portaddr, int queuelim, bool local);

        void open_listening();

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
        void handle_next();
        void shutdown();
};