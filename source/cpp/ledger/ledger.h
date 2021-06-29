#include "../net/net.h"

#pragma once
class Ledger {
    private:
        Server ninet;
        Server nlocal;
        //Server::Client& GUI;
    public:
        Ledger();
        void on_message();
        void start();
        void shutdown();
};