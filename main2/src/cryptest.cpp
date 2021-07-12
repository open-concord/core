#include <string>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#include "../inc/crypt.h"
#include "../inc/hexstr.h"
#include <iostream>

int main() {
	EVP_PKEY* dualkey = EVP_PKEY_new();
	EVP_PKEY* rsa_dualkey = EVP_PKEY_new();
	unsigned char* msg = (unsigned char*)"The quick brown fox jumps over the lazy dog...";
	unsigned char* fromhexmsg;
    size_t fromhexmsg_len = NULL;
    unsigned char* secret = (unsigned char*)"Here, we have a test secret!";
	unsigned char* lockedmsg;
	size_t lockedlen = NULL;
	unsigned char* unlockedmsg;
	size_t unlockedlen = NULL;
	unsigned char* sig;
	size_t siglen = NULL;

	std::cout << msg << std::endl;
	std::cout << rsakeygen(rsa_dualkey) << std::endl;
	std::cout << dsakeygen(dualkey) << std::endl;
    std::cout << "SUCCESSFUL KEYGEN" << std::endl;
	std::cout << lockmessage((const char*) msg, lockedmsg, &lockedlen, dualkey, true, rsa_dualkey, secret) << std::endl;
	std::cout << "CIPHERTEXT LENGTH: " << lockedlen << std::endl;
	std::cout << unlockmessage(lockedmsg, lockedlen, unlockedmsg, &unlockedlen, sig, &siglen, true, rsa_dualkey, secret) << std::endl;
	std::cout << unlockedmsg << std::endl;
	std::cout << verify((const char*) msg, &sig, siglen, dualkey) << std::endl;
	/*
	unsigned char* ek = (unsigned char*) malloc(EVP_PKEY_size(rsa_dualkey));
	int ekl;
	unsigned char* iv = (unsigned char*) malloc(16);
	unsigned char* ciphertext = (unsigned char*) malloc((strlen((const char*) msg) + 17) & (~15));
	int ciphertext_len;

	unsigned char* decrypted_msg = (unsigned char*) malloc((strlen((const char*) msg) + 17) & (~15));
	int decrypted_msg_len;

	std::cout << EVP_PKEY_size(rsa_dualkey) << std::endl;
	std::cout << 
	rsa_env_seal(&rsa_dualkey, msg, strlen((const char*) msg), 
	&ek, &ekl, iv, ciphertext, &ciphertext_len) 
	<< std::endl;
	std::cout << "BEGIN LENS" << std::endl;
	std::cout << ciphertext_len << std::endl;
	std::cout << ekl << std::endl;
	std::cout << to_hexstr(ciphertext, ciphertext_len) << std::endl;
	std::cout << rsa_env_open(rsa_dualkey, ciphertext, ciphertext_len, 
	ek, ekl, iv, decrypted_msg, &decrypted_msg_len) 
	<< std::endl;
	std::cout << decrypted_msg << std::endl;
	*/
	
    std::string hexmsg = to_hexstr(msg, strlen((const char*) msg) + 1);

    std::cout << hexmsg << std::endl;
    std::cout << from_hexstr(hexmsg, fromhexmsg, &fromhexmsg_len) << std::endl;
    std::cout << fromhexmsg << std::endl;

    std::array<std::string, 2> dsakeys = hex_keygen(dsakeygen);
	std::array<std::string, 2> rsakeys = hex_keygen(rsakeygen);
	std::cout << "REACHED KEYGEN" << std::endl;
    //std::cout << keys[0] << std::endl << keys[1] << std::endl;

    //std::string hexlockedmsg = hex_lock("SHALT THOU?", "SECRETO", keys[0]);
    //std::string hexunlockmsg = hex_unlock(hexlockedmsg, "SECRETO", keys[1]);

    std::string hex_locked_msg = hex_lock((const char*) msg, dsakeys[0], true, rsakeys[1], (const char*) secret);
    std::cout << hex_locked_msg << std::endl;
    std::array<std::string, 2> hex_unlocked_msg = hex_unlock(hex_locked_msg, true, rsakeys[0], (const char*) secret);
	std::cout << hex_unlocked_msg[0] << std::endl;
	std::cout << hex_verify(hex_unlocked_msg[0], hex_unlocked_msg[1], dsakeys[1]) << std::endl;
	
    return 0;
}