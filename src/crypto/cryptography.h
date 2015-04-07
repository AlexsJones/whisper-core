/*
 * =====================================================================================
 *
 *       Filename:  cryptography.h
 *
 *    Description:  Encryption/Decryption functions
 *
 *        Version:  1.0
 *        Created:  19/01/15 09:48:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __CRYPTOGRAPHY_H__
#define __CRYPTOGRAPHY_H__
#include <jnxc_headers/jnxtypes.h>
#include <openssl/des.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <time.h>
typedef enum key_type { PUBLIC, PRIVATE } key_type;

RSA *asymmetrical_generate_key(jnx_size length);

RSA *asymmetrical_key_from_string(jnx_char *key, key_type type);

void asymmetrical_destroy_key(RSA *key);

jnx_char *asymmetrical_key_to_string(RSA *keypair,key_type type); 

jnx_char *asymmetrical_encrypt(RSA *keypair,jnx_uint8 *message, jnx_size *out_len);

jnx_char *asymmetrical_decrypt(RSA *keypair,
    jnx_uint8 *message, jnx_size in_len, jnx_size *out_len);

jnx_char *symmetrical_encrypt(jnx_uint8 *key,jnx_uint8 *msg, jnx_size size);

jnx_char *symmetrical_decrypt(jnx_uint8 *key,jnx_uint8 *msg, jnx_size size);

jnx_size generate_shared_secret(jnx_uint8 **buffer);
#endif
