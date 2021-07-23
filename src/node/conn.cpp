// boost
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

// std
#include <string>
#include <map>
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>
#include "../../inc/node.h"

// Connection instance
Conn::Conn(std::map<std::string, FileTree>* pchains, Conn::ptr* plocal_conn, boost::asio::io_context& io_ctx) : parent_chains(pchains), parent_local_conn(plocal_conn), tsock(io_ctx) {
    this->server = true;
}

Conn::ptr Conn::create(std::map<std::string, FileTree>* pchains, Conn::ptr* plocal_conn, boost::asio::io_context& io_ctx) {
    return ptr(new Conn(pchains, plocal_conn, io_ctx));
}
boost::asio::ip::tcp::socket& Conn::socket() {
    return this->tsock;
}

// === async util functions ===
// read from incoming buffer
void Conn::read() {
    boost::asio::async_read(this->tsock,
        boost::asio::buffer(this->incoming_msg),
        boost::bind(
            &Conn::handle,
            this
        )
    );
}

// begin communication
void Conn::initiate_comms(std::string msg) {
    this->server = false;
    this->tsock.async_send(
        boost::asio::buffer(msg, msg.size()),
        boost::bind(
            &Conn::send_done,
            this,
            boost::asio::placeholders::error
            )
    );
} 
// check if we're done (logic-wise)in case of 
void Conn::send_done(const boost::system::error_code& err) {
    if (!err) {
        if (this->done) {
            this->tsock.close();
        } else {
            this->handle();
        }
    } else {
        std::cout << err << "\n";
    }
}

//send a message, async
void Conn::send(std::string msg) {
    this->tsock.async_send(
        boost::asio::buffer(msg, msg.size()),
        boost::bind(
            &Conn::send_done,
            this,
            boost::asio::placeholders::error
        )
    );
}
// === end util ===

// recursive handling :pogU:
void Conn::handle() {
    if (this->incoming_msg.empty()) {
        this->read();
    } else {
        // to change message-specific-logic, modify logic.cpp
        std::string msg = message_logic(this);
        send(msg);
    }
    
}