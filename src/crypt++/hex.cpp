#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/gcm.h>
#include <cryptopp/filters.h>

#include <string>

#include "../../inc/crypt++.h"
using namespace CryptoPP;

std::string hencode(SecByteBlock ibytes) {
    std::string rstr;

    HexEncoder encoder;
    encoder.Put(ibytes, ibytes.size());
    encoder.MessageEnd();

    word64 size = encoder.MaxRetrievable();
    if(size) {
        rstr.resize(size);		
        encoder.Get((byte*)&rstr[0], rstr.size());
    }
    return rstr;
}

std::string hdecode(std::string istr) {
    std::string rstr;
    HexDecoder decoder;
    decoder.Put( (byte*)istr.data(), istr.size() );
    decoder.MessageEnd();

    word64 size = decoder.MaxRetrievable();
    if(size && size <= SIZE_MAX) {
        rstr.resize(size);		
        decoder.Get((byte*)&rstr[0], rstr.size());
    }
    return rstr;
}