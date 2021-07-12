#include <string>
#include <string.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdio.h>

#include "../../inc/hexstr.h"

//most efficient solution, though a bit inelegant.
int hextoi(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return 0;
}

std::string to_hexstr(unsigned char* chars, size_t size) {
    std::stringstream strs;

    for (size_t i = 0; i < size; i++) {
        strs << std::hex << std::setw(2) << std::setfill('0') << (int)chars[i];
    }

    return strs.str();
}

int from_hexstr(std::string data, unsigned char*& outchars, size_t* outsize) {
    //outchars needs to be a buffer, ofc
    assert((data.length() % 2) == 0); //make sure this actually can be decoded.
    size_t size = data.length() / 2;
    outchars = (unsigned char*) malloc(size);
    *outsize = size;

    unsigned char buf[size];
    for (size_t i = 0; i < size; i++) {
        buf[i] = (char) ((hextoi(data[2*i])*16) + hextoi(data[(2*i) + 1]));
        //faster than combining chars.
    }
    memcpy(outchars, buf, size);
    return 1;
}