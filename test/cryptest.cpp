#include <string>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#include "../../inc/crypt.h"
#include "../../inc/hexstr.h"
#include <iostream>

int main() {
	EVP_PKEY* dualkey = EVP_PKEY_new();
	unsigned char* msg = (unsigned char*)"The quick brown fox jumps over the lazy dog...";
	unsigned char* fromhexmsg;
    size_t fromhexmsg_len = NULL;
    unsigned char* secret = (unsigned char*)"Here, we have a test secret!";
	unsigned char lockedmsg[((strlen((const char*) msg) + 2 + 64 + 16) & (~15)) + 128];
	size_t lockedlen = NULL;
	unsigned char unlockedmsg[strlen((const char*) msg) + 1];
	size_t unlockedlen = NULL;

	unsigned char* iv = (unsigned char*)"0123456789012345";

	std::cout << msg << std::endl;
	std::cout << keygen_a(dualkey) << std::endl;
    std::cout << "SUCCESSFUL KEYGEN" << std::endl;
	std::cout << lockmessage((const char*)msg, lockedmsg, &lockedlen, dualkey, secret) << std::endl;
	std::cout << "CIPHERTEXT LENGTH: " << lockedlen << std::endl;
	std::cout << unlockmessage(lockedmsg, lockedlen, unlockedmsg, &unlockedlen, dualkey, secret) << std::endl;
	std::cout << unlockedmsg << std::endl;

    std::string hexmsg = to_hexstr(msg, strlen((const char*) msg) + 1);

    std::cout << hexmsg << std::endl;
    std::cout << from_hexstr(hexmsg, fromhexmsg, &fromhexmsg_len) << std::endl;
    std::cout << fromhexmsg << std::endl;

    std::array<std::string, 2> keys = hex_keygen();
    //std::cout << keys[0] << std::endl << keys[1] << std::endl;

    //std::string hexlockedmsg = hex_lock("SHALT THOU?", "SECRETO", keys[0]);
    //std::string hexunlockmsg = hex_unlock(hexlockedmsg, "SECRETO", keys[1]);

    unsigned char lockedmsg2[((strlen((const char*) msg) + 2 + 64 + 16) & (~15)) + 128];
	size_t lockedlen2 = NULL;
	unsigned char unlockedmsg2[strlen((const char*) msg) + 1];
	size_t unlockedlen2 = NULL;

    std::string hex_locked_msg = hex_lock((const char*) msg, (const char*) secret, keys[0]);
    std::cout << hex_locked_msg << std::endl;
    std::cout << hex_unlock(hex_locked_msg, (const char*) secret, keys[1]) << std::endl;
    return 0;
}