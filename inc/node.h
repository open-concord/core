#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <nlohmann/json.hpp>

#include <iostream>
#include <thread>
#include <memory>
#include <string>
#include <map>
#include <chrono>
#include <vector>

using json = nlohmann::json;

class Conn : public boost::enable_shared_from_this<Conn> {
    private:
        Conn(boost::asio::io_context& io_ctx);
        boost::asio::ip::tcp::socket tsock;   
    public:
        // util vars
        std::map<std::string/*flag*/, json/*content*/> message_context; // to log socket communiation
        std::string incoming_msg; // temp var
        bool done; // close socket (for use in logic function)
        bool server; // which capacity client is currently serving in

        // basically just a shared_ptr of self
        typedef std::shared_ptr<Conn> ptr;
        static ptr create(boost::asio::io_context& io_ctx);
        boost::asio::ip::tcp::socket& socket();
        // async util func
        void initiate_comms(std::string msg);
        void read();
        void send_done(const boost::system::error_code& err, std::size_t bytes);
        void handle();
};

std::string message_logic(Conn *conn_obj);

class Node {
    private:
        // context objs
        boost::asio::io_context io_ctx;
        boost::asio::ssl::context ssl_ctx{boost::asio::ssl::context::tlsv12};

        // *not threadsafe* (like most networking i/o)
        // docs mention using boost::asio::make_strand(ioc), maybe look into that later
        boost::asio::ip::tcp::resolver res{io_ctx};

        // endpoint + max queue
        boost::asio::ip::tcp::endpoint endpoint;

        boost::asio::ip::tcp::acceptor acceptor{io_ctx, endpoint};
        unsigned short int queue;

        // past connection retention
        std::string target_dir;
        struct khost {
            std::string address;
            unsigned short int port;
            std::chrono::high_resolution_clock::time_point last_verfed;
        };
        std::vector<khost> known_hosts;

    public:
        Node(unsigned short int queue, unsigned short int port);

        // start listening
        void start();

        // stop listening
        void stop();

        // passive communication (eg traditional server role)
        void begin_next();
        
        void handle_accept(Conn::ptr new_conn, const boost::system::error_code& err);
        
        // active communication (eg traditional client role)
        void contact(std::string initial_content, std::string ip, int port);
};
