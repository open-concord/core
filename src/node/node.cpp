// boost
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/hof/placeholders.hpp>

// std
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>
#include "../../inc/node.h"

// Connection instance

Conn::Conn(boost::asio::io_context& io_ctx) : tsock(io_ctx) {}

Conn::ptr Conn::create(boost::asio::io_context& io_ctx) {
    return ptr(new Conn(io_ctx));
}
boost::asio::ip::tcp::socket& Conn::socket() {
    return this->tsock;
}
void Conn::handle() {
    // https://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/ref/boost__beast__tcp_stream.html

    // support for async read

    // HANDLE HERE

    // NOTE
    // make sure to add the socket's info to this->khosts
}
// this is really jank, we should have seperate error handlers for each function
void Conn::error_handle(const boost::system::error_code& err) {
    // error handling here
}

// overarching node instance
Node::Node(unsigned short int queue, unsigned short int port) {
    boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::tcp::v4(), port};
    this->queue = queue;
}

void Node::start() {
    // acceptor setup
    this->acceptor.open(this->endpoint.protocol());
    this->acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    this->acceptor.bind(this->endpoint);
    this->acceptor.listen(this->queue);
};

// stop listen
void Node::stop() {
    // should be able to add graceful acceptor shutdown with boost::asio::bind_executor
    // also make sure to close out any still-uncompleted async tasks
    this->acceptor.cancel();
    this->acceptor.close();
};
// awaits new connection and passes message to handler
void Node::begin_next() {
    // create new connection instance
    Conn::ptr new_conn = Conn::create(this->io_ctx);

    // await connection creation
    this->acceptor.async_accept(new_conn->socket(),
        boost::bind(&Node::handle_accept, this, new_conn,
        boost::asio::placeholders::error)
    );
};
// util func for async-call timing
void Node::handle_accept(Conn::ptr new_conn, const boost::system::error_code& err) {
    if (!err) {
        new_conn->handle();
        begin_next();
    } else {
        // kinda jank, we should really write seperate error handlers for each function
        new_conn->error_handle(err);
    }
};

// intiates contact with another ws
void Node::contact(std::string content, std::string ip, int port) {
    // target info
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
    // socket creation
    boost::beast::ssl_stream<boost::beast::tcp_stream> sock {io_ctx, ssl_ctx};

    sock.next_layer().connect(ep);

    // can use sock.next_layer to send stuff (may be sync, idk about async outgoing)
};
