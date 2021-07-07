#include <string>
#include <vector>
#include <chrono>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>

using json = nlohmann::json;

std::string sock_read(boost::asio::ip::tcp::socket& sock) {
    //THIS IS USING A STRING EOM, NEEDS TO BE CHANGED!
    boost::asio::streambuf buffer;
    boost::asio::ip::read(
        sock, 
        buffer, 
        boost::asio::transfer_all()
    );
    return buffer_cast<const char*>(buf.data());
}

void sock_write(boost::asio::ip::tcp::socket& sock, std::string data) {
    boost::asio::write(
            sock,
            boost::asio::buffer(data.c_str(), data.size()), 
            boost::asio::transfer_all()
    );
}

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

    sock_write(sock, "CONCORDID:HOST"); //identify role
    if (sock_read(sock) != "CONCORDID:CLIENT") return;
    sock_write(sock, "CONCORDCHAIN:READY"); //communicate readiness
}