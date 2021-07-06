#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream> // For cout
#include <unistd.h> // For read
#include <string>

#include "../../inc/net.h"


Client::Client (Server* h) : hub(h) {
    // pickup the next connection in queue
    auto addrlen = sizeof(hub->addr);

    // accept new connection
    this->connection = accept(hub->sockfd, (struct sockaddr *) &hub->addr, (socklen_t*)&addrlen);

    if (this->connection < 0) {
        std::cout << "Error while grabbing connection: " << errno << std::endl;
        // exit handling here
        exit(EXIT_FAILURE);
    }

    // read connection
    auto bytesread = read(connection, buffer, 255);
}

// takes callback
void Client::handle(void (* handleFunctionPointer)(std::string)) {
    handleFunctionPointer(std::string(this->buffer));
}

void Client::send_connection(std::string msg) {
    send(connection, msg.c_str(), msg.size(), 0);
}

void Client::close_connection(std::string receipt) {
    // end communication w/ receipt
    send(connection, receipt.c_str(), receipt.size(), 0);
    // close socket connection
    close(connection);
}