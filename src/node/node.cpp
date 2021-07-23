// boost
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/hof/placeholders.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// std
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>
#include "../../inc/node.h"
#include "../../inc/tree.h"

namespace fs = boost::filesystem;

//a utility from https://stackoverflow.com/questions/5043403/listing-only-folders-in-directory
std::vector<std::string> get_directories(const std::string& s)
{
    std::vector<std::string> r;
    for(auto& p : fs::recursive_directory_iterator(s))
        if (is_directory(p))
            r.push_back(p.path().string());
    return r;
}

// overarching node instance
Node::Node(int queue, unsigned short int port, std::string chains_dir, std::vector<std::string> desired_trips) {
    boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::tcp::v4(), port};
    this->queue = queue;
    this->chains_dir = chains_dir;
    if ((this->chains_dir).back() != '/') this->chains_dir += "/";
    for (auto dtrip : desired_trips) fs::create_directory(this->chains_dir + dtrip);
    for (auto dir : get_directories(this->chains_dir)) {
        if (dir.length() == 32) { //tripcode dirs
            (this->chains)[dir].load(this->chains_dir + dir);
        }
    }
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
    this->acceptor.close();
};

// shutdown **THIS CLOSES ALL ASYNC OPERATIONS, ONLY USE IN EMERGENCIES**
void Node::shutdown() {
    this->acceptor.close();
    this->acceptor.cancel();
}

// awaits new connection and passes message to handler
void Node::begin_next() {
    // create new connection instance
    Conn::ptr new_conn = Conn::create(&(this->chains), &(this->local_conn), this->io_ctx);

    // await connection creation
    this->acceptor.async_accept(new_conn->socket(),
        boost::bind(
            &Node::handle_accept,
            this,
            new_conn,
            boost::asio::placeholders::error
        )
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

// make a local socket
void Node::make_local(int port) {
    json init = {
        {"FLAG", "RUNNING"},
        {"CONTENT", nullptr}
    };
    this->local_conn = Node::contact(init.dump(), "127.0.0.1", port);
};

// intiates contact with another ws
Conn::ptr Node::contact(std::string initial_content, std::string ip, int port) {
    // target info
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
    // socket creation
    Conn::ptr new_conn = Conn::create(&(this->chains), &(this->local_conn), this->io_ctx);
    // not capable of doing async mental gymnastics to get async_connect hooked up
    // but I also don't want to write a helper func
    // *fix in alpha*
    
    new_conn->socket().connect(ep);
    new_conn->initiate_comms(initial_content);
    return new_conn;
};

//special version of contact with init variables
void Node::chain_contact(std::string chaintrip, size_t k, std::string ip, int port) {
    json message = {
        {"FLAG", "READY"},
        {"CONTENT", {
            {"chain", chaintrip},
            {"k", k}
        }}
    };
    Conn::ptr conn = contact(message.dump(), ip, port);
    (conn->message_context).chain_trip = chaintrip;
    (conn->message_context).k = k;
}