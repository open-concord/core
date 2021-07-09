// boost
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/hof/placeholders.hpp>
#include <boost/asio/spawn.hpp>

// std
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>
#include "../../inc/node.h"

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
        new_conn->read();
        begin_next();
    } else {
        std::cout << err << "\n";
    }
};

// intiates contact with another ws
void Node::contact(std::string initial_content, std::string ip, int port) {
    // target info
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
    // socket creation
    Conn::ptr new_conn = Conn::create(this->io_ctx);
    // not capable of doing async mental gymnastics to get async_connect hooked up
    // but I also don't want to write a helper func
    // *fix in alpha*
    new_conn->socket().connect(ep);
    new_conn->initiate_comms(initial_content);
};

