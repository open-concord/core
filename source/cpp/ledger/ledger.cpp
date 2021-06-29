#include <string>
#include <iostream>


#include "../net/net.h"
#include "ledger.h"

Ledger::Ledger() : ninet(Server(1337, 20, false)) : nlocal(Server(1338, 2, true) : {
    Server ninet = Server(1337, 20, false);
    Server nlocal = Server(1338, 2, true);
    Server::Client& GUI(Server nlocal);
}

/**
class Ledger {
    private:
        Server ninet = Server(1337, 20, false);
        Server nlocal = Server(1338, 2, true);
        Server::Client& GUI;
    public:
        Ledger() : GUI(nlocal.handle_next()){
            std::cout << "GUI ADDR connected" << "\n";
        }

        void on_message() {
            GUI.send_connection("HI! JUST TESTING");
        }

        void start() {
            // run hub
            ninet.open_listening();
            //ninet.handle_next();
        }
        void shutdown() {
            ninet.shutdown();
        }

};