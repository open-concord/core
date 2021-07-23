#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/any.hpp>

#include <nlohmann/json.hpp>

#include "tree.h"

#include <iostream>
#include <thread>
#include <pthread.h>
#include <memory>
#include <string>
#include <map>
#include <chrono>
#include <vector>

using json = nlohmann::json;

std::vector<json> chain_search(std::vector<std::vector<std::string>> chain, char message_type, std::string target_trip, std::string key, bool (*filter)(json), size_t start_b = -1, size_t end_b = -1);

bool type_filter(char qtype, json data);

struct user_keys {
    std::map<std::string, std::string> server_keys;
    std::string dsa_pri_key;
    std::string rsa_pri_key;
}

struct conn_context {
    std::vector<std::vector<std::string>> wchain;
    std::string chain_trip;
    size_t lastbi;
    size_t k;
    size_t pow_min;
    std::map<std::string, user_keys> user_keys_map;
};

class Conn : public boost::enable_shared_from_this<Conn> {
    private:
        Conn(std::map<std::string, FileTree>*, boost::asio::io_context& io_ctx);
        boost::asio::ip::tcp::socket tsock;   
    public:
        std::map<std::string, FileTree>* parent_chains;
        Conn::ptr* parent_local_conn;

        conn_context message_context;
        // util vars
         // to log socket communiation
        std::string incoming_msg; // temp var
        bool done; // close socket (for use in logic function)
        bool server; // which capacity client is currently serving in
        bool local; // is local connection?

        // basically just a shared_ptr of self
        typedef std::shared_ptr<Conn> ptr;
        static ptr create(std::map<std::string, FileTree>*, boost::asio::io_context& io_ctx);
        boost::asio::ip::tcp::socket& socket();
        // async util func
        void initiate_comms(std::string msg);
        void read();
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
        std::string chains_dir;
        struct khost {
            std::string address;
            unsigned short int port;
            std::chrono::high_resolution_clock::time_point last_verified;
        };
        std::vector<khost> known_hosts;

        Conn:ptr local_conn;
    public:
        std::map<std::string /*trip*/, FileTree /*chain model*/> chains;

        Node(int queue, unsigned short int port, std::string chains_dir, std::vector<std::string> desired_trips);

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

        // make a local connection (for use with GUI)
        void make_local(int port);

        bool type_filter(char type, json cont);
};
