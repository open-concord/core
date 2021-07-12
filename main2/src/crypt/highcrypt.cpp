#include <iostream>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cassert>

#include "../../inc/crypt.h"

int lockmessage(const char* plaintext, unsigned char*& ciphertext, size_t* ciphertextlen, EVP_PKEY* sigprikey, bool use_asymm, EVP_PKEY* encpubkey, unsigned char* serversecret) {
	unsigned char* sig = NULL;
	unsigned char** sigref = &sig;
	unsigned char iv[16];
	size_t siglen = NULL;
	size_t* siglenref = &siglen;

	if (sign(plaintext, sigref, siglenref, sigprikey) != 1) return 0;

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

	size_t ciphertext_buf_mlen = ((unifiedlen + 16) & (~15)); //unified len rounded up over 16
	if (use_asymm) {
		size_t ksize = EVP_PKEY_size(encpubkey);
		std::cout << ksize << std::endl;
		ciphertext = (unsigned char*) malloc(ciphertext_buf_mlen + ksize + 16);
		unsigned char* encrypted_key = ciphertext + ciphertext_buf_mlen;
		int encrypted_key_len;
		std::cout << "REACHED SEAL" << std::endl;
		if (!rsa_env_seal(&encpubkey, unified, unifiedlen, 
			&encrypted_key, &encrypted_key_len, iv, ciphertext, 
			reinterpret_cast<int*>(ciphertextlen)
		)) return 0;
		std::cout << "FINISHED SEAL" << std::endl;
		*ciphertextlen += ksize; //treat key as part of ciphertext
		assert(ksize == (size_t) encrypted_key_len);
	}
	else {
		ciphertext = (unsigned char*) malloc(ciphertext_buf_mlen + 16);
		if (!RAND_bytes(iv, 16)) return 0;
		if (symm_encrypt(unified, unifiedlen, serversecret, iv, ciphertext, ciphertextlen) != 1) return 0;
	}
	//This code literally just puts the full iv after the ciphertext
	memcpy(ciphertext + (*ciphertextlen), iv, 16);

	return 1;
}

int unlockmessage(unsigned char* ciphertext, size_t ciphertextlen, unsigned char*& plaintext, size_t* plaintextlen, unsigned char*& sig, size_t* siglen, bool use_asymm, EVP_PKEY* encpubkey, unsigned char* serversecret) {
	
	//ciphertextlen does NOT include iv.
	//AES plaintext should always be shorter than ciphertext. This is a little hacky.
	unsigned char* unified = (unsigned char*) malloc(ciphertextlen);
	unsigned char iv[16];
	size_t unifiedlen = NULL;

	//Copy the IV from the end of the ciphertext char array
	//Remember that our ciphertextlen is the length of the substance, not including the IV
	//Really important that you don't just take the length of the char array
	memcpy(iv, ciphertext + ciphertextlen, 16);

	if (use_asymm) {
		size_t ksize = EVP_PKEY_size(encpubkey);
		if (!rsa_env_open(encpubkey, ciphertext, (int) ciphertextlen - ksize,
		ciphertext + ciphertextlen - ksize, (int) ksize, iv,
		unified, reinterpret_cast<int*>(&unifiedlen))) return 0;
	}
	else {
		if (symm_decrypt(ciphertext, ciphertextlen, serversecret, iv, unified, &unifiedlen) != 1) return 0;
	}
	unsigned char firstchar[1];

	memcpy(firstchar, unified, 1);

	*siglen = (size_t) firstchar[0];
	sig = (unsigned char*) malloc(*siglen);

	memcpy(sig, unified + 1, *siglen);

	*plaintextlen = (unifiedlen - 1) - *siglen;
	plaintext = (unsigned char*) malloc(*plaintextlen);

	memcpy(plaintext, unified + 1 + *siglen, (unifiedlen - 1) - *siglen);

	//return verify((const char*) plaintext, &sig, siglen, userpubkey);
	return 1;
}