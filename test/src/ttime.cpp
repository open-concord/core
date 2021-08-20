#include "../../src/tree/time_enc.cpp"
#include <iostream>

int main() {
    unsigned long long raw_time = get_raw_time();
    std::string str_time = raw_time_to_string(raw_time);
    unsigned long long out_time = string_to_raw_time(str_time);
    std::cout << raw_time << std::endl;
    std::cout << out_time << std::endl;
    return 0;
}