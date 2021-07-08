#include <array>
#include <string>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
 #include <openssl/bio.h>
  #include <openssl/pem.h>
#include "../../inc/hexstr.h"
#include "../../inc/crypt.h"

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

std::string hex_lock(std::string data, std::string secret, std::string prikey) {
    BIO* pribuf;
    if (!set_BIO_data(pribuf, prikey)) return NULL;
    EVP_PKEY* evp_prikey = PEM_read_bio_PrivateKey(pribuf, NULL, NULL, NULL);

    unsigned char locked_chars[((strlen(data.c_str()) + 2 + 64 + 16) & (~15)) + 128];
    size_t locked_chars_len = NULL;

    if (!lockmessage(data.c_str(), locked_chars, &locked_chars_len, evp_prikey, (unsigned char*) secret.c_str())) return NULL;

    return to_hexstr(locked_chars, locked_chars_len + 128); // add 128 to account for IV
}

std::string hex_unlock(std::string data, std::string secret, std::string pubkey) {
    BIO* pubbuf;
    if (!set_BIO_data(pubbuf, pubkey)) return NULL;
    EVP_PKEY* evp_pubkey = PEM_read_bio_PUBKEY(pubbuf, NULL, NULL, NULL);
    unsigned char* locked_chars;
    size_t locked_chars_len = NULL;

    unsigned char unlocked_chars[(data.length() / 2) - 2 - 64 - 16 - 128];
    size_t unlocked_chars_len = NULL;
    if (!from_hexstr(data, locked_chars, &locked_chars_len)) return NULL;
    if (!unlockmessage(locked_chars, locked_chars_len - 128, unlocked_chars, &unlocked_chars_len, evp_pubkey, (unsigned char*) secret.c_str())) return NULL; //remove 128 from size to account for IV
    return std::string((const char*) unlocked_chars);
}

std::array<std::string, 2> hex_keygen() {
    EVP_PKEY* dualkey = EVP_PKEY_new();
    if (!keygen_a(dualkey)) return {NULL, NULL};
    BIO* pribuf = BIO_new(BIO_s_mem());
    BIO* pubbuf = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PrivateKey(pribuf, dualkey, NULL, NULL, 0, NULL, NULL)) return {NULL, NULL};
    if (!PEM_write_bio_PUBKEY(pubbuf, dualkey)) return {NULL, NULL};
    std::array<std::string, 2> keys = {extract_BIO_data(pribuf), extract_BIO_data(pubbuf)};
    return keys;
}