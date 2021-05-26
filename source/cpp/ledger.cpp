#include "networking.cpp"

class Ledger {
    private:
        Server mynetworking;
    public:
        void start() {
            // run hub
            mynetworking.create_socket();
            mynetworking.open_listening();
            mynetworking.handle_next();
        }
        void shutdown() {
            mynetworking.shutdown();
        }

};