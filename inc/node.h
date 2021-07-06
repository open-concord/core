#include <string>
#include <vector>
#include <chrono>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <thread>

class Node {
    private:
        boost::asio::io_service local_io_service;
        boost::asio::ip::tcp::acceptor local_acceptor;
        std::string target_dir;
        struct khost {
            std::string address;
            int port;
            std::chrono::high_resolution_clock::time_point last_verfed;
        }
        std::vector<khost> known_hosts;
        void start_accept();
        void handle_accept(boost:asio::ip::tcp::socket& handle_socket);
    public:
        Node();

        void start(int accepting_port);

        void stop();
};