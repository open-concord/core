#include <cmath>
#include <ctime>
#include <string>

/** 
* I genuinely hate messing w/ timestamping
* I'm dumping all the ugly code into these functions
*/

// really ugly solution for converting back, but I'm tired, so I'll make something better tmr
//https://stackoverflow.com/questions/42854679/c-convert-given-utc-time-string-to-local-time-zone



// all time is sent out UTC
std::string get_time() {
    std::time_t time_obj = std::time(0);
    std::tm* time_gm = std::gmtime(&time_obj);
    char temp_buf[42];
    std::strftime(temp_buf, 42, "%Y-%m-%dT%X%Z", time_gm);
    return temp_buf;
}

// Converts given time (UTC) to local
std::string convert_to_local() {
    return;
}
