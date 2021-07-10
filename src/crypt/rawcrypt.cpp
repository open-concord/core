//#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <openssl/evp.h> 
#include <openssl/dsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/err.h>

#include "../../inc/crypt.h"

//This code still has some comments in it from the openssl docs, though I had to modify it a ton. They may not be accurate, just a heads up.
//Should probably edit the comments to be accurate at some point, ig. Also worth giving credit here, though, as it's definitely not edited beyond recognition.

int sign(const char* msg, unsigned char** sig, size_t* slen, EVP_PKEY* prikey)
{
    EVP_MD_CTX* mdctx = NULL;
    int ret = 0;

    *sig = NULL;

    /* Create the Message Digest Context */
    if (!(mdctx = EVP_MD_CTX_create())) return 0;

    /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
    if (1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, prikey)) return 0;

    /* Call update with the message */
    if (1 != EVP_DigestSignUpdate(mdctx, msg, strlen(msg))) return 0;

    /* Finalise the DigestSign operation */
    /* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
     * signature. Length is returned in slen */
    if (1 != EVP_DigestSignFinal(mdctx, NULL, slen)) return 0;
    /* Allocate memory for the signature based on size in slen */
    if (!(*sig = (unsigned char*)OPENSSL_malloc(sizeof(unsigned char) * (*slen)))) return 0;
    /* Obtain the signature */
    if (1 != EVP_DigestSignFinal(mdctx, *sig, slen)) return 0;

    /* Success */
    ret = 1;

    /* Clean up */
    if (*sig && !ret) OPENSSL_free(*sig);
    if (mdctx) EVP_MD_CTX_destroy(mdctx);

    return ret;
}

int verify(const char* msg, unsigned char** sig, size_t slen, EVP_PKEY* pubkey) {

    EVP_MD_CTX* mdctx = NULL;
    int ret = 0;

    if (!(mdctx = EVP_MD_CTX_create())) return 0;
    /* Initialize `key` with a public key */
    if (1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pubkey)) return 0;

    /* Initialize `key` with a public key */
    if (1 != EVP_DigestVerifyUpdate(mdctx, msg, strlen(msg))) return 0;

    if (1 == EVP_DigestVerifyFinal(mdctx, *sig, slen))
    {
        ret = 1;
    }
    else
    {
        ret = -1;
    }

    /* Clean up */
    if (*sig && !ret) OPENSSL_free((void*)*sig);
    if (mdctx) EVP_MD_CTX_destroy(mdctx);

    return ret;
}

int paramgen(EVP_PKEY* params) {
    /* Create the context for generating the parameters */
    EVP_PKEY_CTX* pctx;
    int ret = 0;
    if (!(pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_DSA, NULL))) return 0;

    if (!EVP_PKEY_paramgen_init(pctx)) return 0;

    if (!EVP_PKEY_CTX_set_dsa_paramgen_bits(pctx, 2048)) return 0;

    if (!EVP_PKEY_paramgen(pctx, &params)) return 0;

    ret = 1;

    return ret;
}

int keygen_b(EVP_PKEY* params, EVP_PKEY* key) {
    EVP_PKEY_CTX* kctx;
    int ret = 0;
    if (!(kctx = EVP_PKEY_CTX_new(params, NULL))) return 0;

    if (!EVP_PKEY_keygen_init(kctx)) return 0;

    if (!EVP_PKEY_keygen(kctx, &key)) return 0;

    ret = 1;

    return ret;
}

int keygen_a(EVP_PKEY* key) {
    EVP_PKEY* params = EVP_PKEY_new();
    int ret = 0;

    if (1 != paramgen(params)) return 0;
    if (1 != keygen_b(params, key)) return 0;

    ret = 1;

    return ret;
}

int symm_encrypt(unsigned char* plaintext, size_t plaintextlen, unsigned char* key,
    unsigned char* iv, unsigned char* ciphertext, size_t* ciphertextlen)
{
    EVP_CIPHER_CTX* ctx;
    int ret = 0;
    int len;
    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        return 0;
    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        return 0;
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintextlen))
        return 0;
    *ciphertextlen = len;
    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        return 0;
    *ciphertextlen += len;
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    ret = 1;
    return ret;
}

int symm_decrypt(unsigned char* ciphertext, size_t ciphertextlen, unsigned char* key,
    unsigned char* iv, unsigned char* plaintext, size_t* plaintextlen)
{
    int ret = 0;
    EVP_CIPHER_CTX* ctx;
    int len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) return 0;

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return 0;

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertextlen)) return 0;
    *plaintextlen = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) return 0;
    *plaintextlen += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return 1;
}

int asymm_encrypt() {

}