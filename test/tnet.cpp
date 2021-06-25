#include "../source/cpp/util/net.cpp"

int main () {
    Server test(1337, 20, true);
    test.open_listening();
    test.handle_next();
    return 0;
}