//#include <iostream>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <string>
#include <string.h>
#include <stdio.h>

#include "../../inc/crypt.h"

int lockmessage(const char* plaintext, unsigned char* ciphertext, size_t* ciphertextlen, EVP_PKEY* userprikey, unsigned char* serversecret) {
	unsigned char* sig = NULL;
	unsigned char** sigref = &sig;
	unsigned char iv[128];
	size_t siglen = NULL;
	size_t* siglenref = &siglen;

	if (!RAND_bytes(iv, 128)) return 0;

	if (sign(plaintext, sigref, siglenref, userprikey) != 1) return 0;

	unsigned char firstchar[1] = { (unsigned char)siglen };

	//  1 byte for siglen, siglen bytes for sig, and strlen(plaintext) + 1 bytes for plaintext.
	const size_t unifiedlen = 2 + siglen + strlen(plaintext);
	unsigned char* unified = (unsigned char*) malloc(unifiedlen);
	//unsigned char* sig2 = (unsigned char*) malloc(siglen); debug versions of sig
	//unsigned char* sig3 = (unsigned char*) malloc(siglen);

	memcpy(unified, (unsigned char*) firstchar, 1);
	memcpy(unified + 1, sig, siglen);
	memcpy(unified + 1 + siglen, (unsigned char*) plaintext, strlen(plaintext) + 1);

	//memcpy(sig2, unified + 1, siglen); debug copy

	//Info for debugging
	/*
	std::cout << "VERIFY A: " << verify(plaintext, &sig, siglen, userprikey) << std::endl;
	std::cout << "VERIFY B: " << verify(plaintext, &sig2, siglen, userprikey) << std::endl;
	std::cout << "UNIFIED LENGTH: " << unifiedlen << std::endl;
	//*/

	if (encrypt(unified, unifiedlen, serversecret, iv, ciphertext, ciphertextlen) != 1) return 0;

	//This code literally just puts the full iv after the ciphertext
	memcpy(ciphertext + (*ciphertextlen), iv, 128);

	return 1;
}

int unlockmessage(unsigned char* ciphertext, size_t ciphertextlen, unsigned char* plaintext, size_t* plaintextlen, EVP_PKEY* userpubkey, unsigned char* serversecret) {
	
	//AES plaintext should always be shorter than ciphertext. This is a little hacky.
	unsigned char* unified = (unsigned char*) malloc(ciphertextlen);
	unsigned char iv[128];
	size_t unifiedlen = NULL;

	//Copy the IV from the end of the ciphertext char array
	//Remember that our ciphertextlen is the length of the substance, not including the IV
	//Really important that you don't just take the length of the char array
	memcpy(iv, ciphertext + ciphertextlen, 128);

	if (decrypt(ciphertext, ciphertextlen, serversecret, iv, unified, &unifiedlen) != 1) return 0;

	unsigned char firstchar[1];

	memcpy(firstchar, unified, 1);

	size_t siglen = (size_t) firstchar[0];
	unsigned char* sig = (unsigned char*) malloc(siglen);

	memcpy(sig, unified + 1, siglen);

	memcpy(plaintext, unified + 1 + siglen, (unifiedlen - 1) - siglen);

	return verify((const char*) plaintext, &sig, siglen, userpubkey);
}