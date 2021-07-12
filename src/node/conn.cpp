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
Conn::Conn(boost::asio::io_context& io_ctx) : tsock(io_ctx) {}

Conn::ptr Conn::create(boost::asio::io_context& io_ctx) {
    return ptr(new Conn(io_ctx));
}
boost::asio::ip::tcp::socket& Conn::socket() {
    return this->tsock;
}

// === async util functions ===
// read from incoming buffer
void Conn::read() {
    boost::asio::async_read(
        this->tsock,
        boost::asio::buffer(this->incoming_msg),
        boost::bind(&Conn::handle, shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred
        )
    );
}

// begin communication
void Conn::initiate_comms(std::string msg) {
    this->tsock.async_send(
        boost::asio::buffer(msg, msg.size()),
        &Conn::send_done
    );
} 
// check if we're done (logic-wise)in case of 
void Conn::send_done(const boost::system::error_code& err, std::size_t bytes) {
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
// === end util ===

// recursive handling :pogU:
void Conn::handle() {
    if (this->incoming_msg.empty()) {
        this->read();
    } else {
        // to change message-specific-logic, modify logic.cpp
        std::string msg = message_logic(this);
        this->tsock.async_send(
            boost::asio::buffer(msg, msg.size()),
            &Conn::send_done
        );
    }
    
}