#include "server.cpp"

/*
class Ledger {
    private:

    public:
}
*/
int main() {
    // just testing networking
    Server testing;
    // here we go
    testing.create_socket();
    testing.start_listen();
    testing.handle();
    testing.shutdown();
    return 0;
}