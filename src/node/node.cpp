#include <string>
#include <vector>
#include <chrono>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <thread>

Node::Node() {
    (this->local_io_service).run();
}

void Node::start(int accepting_port) {
    this->local_acceptor = boost::asio::ip::tcp::acceptor(this->local_io_service, tcp::endpoint(tcp::v4(), accepting_port));   

    start_accept();
}

void Node::stop() {
    (this->local_acceptor).cancel();
}

void Node::start_accept() {
    boost::asio::ip::tcp::socket handle_socket(this->local_io_service);
    (this->local_acceptor).async_accept(handle_socket, boost::bind(&Node::handle, this, handle_socket));
}

void Node::handle(boost::asio::ip::tcp::socket& sock) {
    start_accept(); //make sure new requests also get handled.

    //handling time
}