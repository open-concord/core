#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>

#include <nlohmann/json.hpp>

#include "tree.h"

#include <iostream>
#include <thread>
#include <pthread.h>
#include <memory>
#include <string>
#include <map>
#include <unordered_set>
#include <chrono>
#include <vector>

using json = nlohmann::json;

struct conn_context {
    std::vector<block> new_blocks;
    std::string chain_trip;
    std::unordered_set<std::string> seen_hashes;
    std::unordered_set<std::string> last_layer;
    size_t k;
    size_t pow_min;
    bool first_layer = true;
};

class Conn : public boost::enable_shared_from_this<Conn> {
    public:
        // basically just a shared_ptr of self
        typedef std::shared_ptr<Conn> ptr;
    private:
        Conn(std::map<std::string, Tree>*, boost::asio::io_context& io_ctx);
        boost::asio::ip::tcp::socket tsock;   
    public:
        std::map<std::string, Tree>* parent_chains;

        conn_context message_context;
        // util vars
         // to log socket communiation
        std::string incoming_msg; // temp var
        bool done; // close socket (for use in logic function)
        bool server; // which capacity client is currently serving in
        bool local; // is local connection?

        static ptr create(std::map<std::string, Tree>*, boost::asio::io_context& io_ctx);
        boost::asio::ip::tcp::socket& socket();
        // async util func
        void initiate_comms(std::string msg);
        void read();
        void send(std::string msg);
        void send_done(const boost::system::error_code& err);
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
        int queue;

        // past connection retention
        struct khost {
            std::string address;
            unsigned short int port;
            std::chrono::high_resolution_clock::time_point last_verified;
        };
        std::vector<khost> known_hosts;
    public:
        std::map<std::string /*trip*/, Tree /*chain model*/> chains;

        Node(int queue, unsigned short int port, std::map<std::string, Tree> cm);

        // start listening
        void start();

        // stop listening
        void stop();
        // shutdown **THIS CLOSES ALL ASYNC OPERATIONS, ONLY USE IN EMERGENCIES**
        void shutdown();

        // passive communication (eg traditional server role)
        void begin_next();
        
        void handle_accept(Conn::ptr new_conn, const boost::system::error_code& err);
        
        // active communication (eg traditional client role)
        Conn::ptr contact(std::string initial_content, std::string ip, int port);

        void chain_contact(std::string chaintrip, size_t k, std::string ip, int port);
};
