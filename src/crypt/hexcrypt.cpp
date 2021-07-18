#include <array>
#include <string>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include "../../inc/hexstr.h"
#include "../../inc/crypt.h"
#include <iostream>

//this file should be overhauled, as there are *much* more secure ways of transmitting keys than symmetrically-encrypted strings.
//code here just keeps with the string-focused architecture we have at this early stage.

std::string extract_BIO_data(BIO* bio) {
    unsigned char* buf;
    long size = BIO_get_mem_data(bio, &buf);
    //unsigned char* outbuf = malloc(size); this is an alternative if direct reading doesn't work. tbimplemented.
    //memcpy(outbuf, buf, size);
    return to_hexstr(buf, size);
}

int set_BIO_data(BIO*& bio, std::string data_hex_str) {
    unsigned char* buf;
    size_t bufsize = NULL;
    if (!from_hexstr(data_hex_str, buf, &bufsize)) return 0;
    bio = BIO_new_mem_buf((void*) buf, bufsize);
    return 1;
}

std::string hex_lock(std::string data, std::string sigprikey, bool use_asymm, std::string encpubkey, std::string secret) {
    BIO* dsa_pribuf;
    if (!set_BIO_data(dsa_pribuf, sigprikey)) return NULL;
    EVP_PKEY* evp_dsa_prikey = PEM_read_bio_PrivateKey(dsa_pribuf, NULL, NULL, NULL);
    EVP_PKEY* evp_rsa_pubkey;
    if (use_asymm) {
        BIO* rsa_pubbuf;
        if (!set_BIO_data(rsa_pubbuf, encpubkey)) return NULL;
        evp_rsa_pubkey = PEM_read_bio_PUBKEY(rsa_pubbuf, NULL, NULL, NULL);
    }
    unsigned char* locked_chars;
    size_t locked_chars_len = NULL;
    std::cout << "REACHED OP" << std::endl;
    if (!lockmessage(data.c_str(), locked_chars, &locked_chars_len, evp_dsa_prikey, 
        use_asymm, evp_rsa_pubkey, (unsigned char*) secret.c_str())) return NULL;
    std::cout << locked_chars_len << std::endl;

    return to_hexstr(locked_chars, locked_chars_len); //account for IV
}

std::array<std::string, 2> hex_unlock(std::string data, bool use_asymm, std::string encprikey, std::string secret) {
    EVP_PKEY* evp_rsa_prikey;
    if (use_asymm) {
        BIO* rsa_pribuf;
        if (!set_BIO_data(rsa_pribuf, encprikey)) return {NULL, NULL};
        evp_rsa_prikey = PEM_read_bio_PrivateKey(rsa_pribuf, NULL, NULL, NULL);
    }
    unsigned char* locked_chars;
    size_t locked_chars_len = NULL;

    unsigned char* unlocked_chars;
    size_t unlocked_chars_len = NULL;

    unsigned char* sig;
    size_t sig_len = NULL;

    if (!from_hexstr(data, locked_chars, &locked_chars_len)) return {NULL, NULL};
    if (!unlockmessage(locked_chars, locked_chars_len, 
        unlocked_chars, &unlocked_chars_len, sig, &sig_len, use_asymm, 
        evp_rsa_prikey, (unsigned char*) secret.c_str())
    ) return {NULL, NULL}; //remove 128 from size to account for IV
    return {std::string((const char*) unlocked_chars), to_hexstr(sig, sig_len)};
}

int hex_verify(std::string data, std::string sig, std::string sigpubkey) {
    BIO* dsa_pubbuf;
    if (!set_BIO_data(dsa_pubbuf, sigpubkey)) return 0;
    EVP_PKEY* evp_dsa_pubkey = PEM_read_bio_PUBKEY(dsa_pubbuf, NULL, NULL, NULL);

    unsigned char* sigbuf;
    size_t siglen;
    if (!from_hexstr(sig, sigbuf, &siglen)) return 0;
    return verify(data.c_str(), &sigbuf, siglen, evp_dsa_pubkey);
}

std::array<std::string, 2> hex_keygen(int (*keygenfunc)(EVP_PKEY*)) {
    EVP_PKEY* dualkey = EVP_PKEY_new();
    if (!(keygenfunc(dualkey))) return {NULL, NULL};
    BIO* pribuf = BIO_new(BIO_s_mem());
    BIO* pubbuf = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PrivateKey(pribuf, dualkey, NULL, NULL, 0, NULL, NULL)) return {NULL, NULL};
    if (!PEM_write_bio_PUBKEY(pubbuf, dualkey)) return {NULL, NULL};
    std::array<std::string, 2> keys = {extract_BIO_data(pribuf), extract_BIO_data(pubbuf)};
    return keys;
}