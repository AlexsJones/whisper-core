/*
 * =====================================================================================
 *
 *       Filename:  cryptography.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19/01/15 10:59:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "cryptography.h"
#include <time.h>
#include <string.h>
#include <jnxc_headers/jnxcheck.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wpointer-sign"

RSA *asymmetrical_generate_key(jnx_size length) {
  srand(time(NULL));
  return RSA_generate_key(length,3,NULL,NULL);
}
void asymmetrical_destroy_key(RSA *key) {
  if(key != NULL) RSA_free(key);
}
RSA *asymmetrical_key_from_string(jnx_char *string, key_type type) { 
  BIO *key = BIO_new_mem_buf((void*)string,strlen(string));
  RSA *rsa = RSA_new();
  switch(type) {
    case PUBLIC:
    PEM_read_bio_RSAPublicKey(key,&rsa,0,NULL);

    break;
    case PRIVATE:
      JNX_LOG(NULL,"asymmetrical private key from string not supported!");
  }
  BIO_free(key);
  return rsa;
}
jnx_char *asymmetrical_key_to_string(RSA *keypair,key_type type) {

  BIO *key = BIO_new(BIO_s_mem());
  switch(type) {
    case PUBLIC:
      PEM_write_bio_RSAPublicKey(key,keypair);
      break;

    case PRIVATE:
      PEM_write_bio_RSAPrivateKey(key,keypair,NULL,NULL,0,NULL,NULL);
      break;
  }
  jnx_size len = BIO_pending(key);
  jnx_char *skey = malloc(len + 1);
  jnx_size read = BIO_read(key,skey,len);
  skey[len] = '\0';
  BIO_free(key);
  return skey;
}
jnx_char *asymmetrical_encrypt(RSA *keypair, jnx_uint8 *message, jnx_size \
    *out_len) {
  jnx_char *encrypted_message = malloc(RSA_size(keypair));
  bzero(encrypted_message, RSA_size(keypair));
  char *err = malloc(30);

  if((*out_len = RSA_public_encrypt(strlen(message) + 1, message,
          encrypted_message, keypair, RSA_PKCS1_OAEP_PADDING)) == -1) {
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(),err);
    printf("%s\n",err);
    free(err);
    return NULL;
  }
  free(err);
  return encrypted_message;
}
jnx_char *asymmetrical_decrypt(RSA *keypair, jnx_uint8 *message, \
    jnx_size in_len, jnx_size *out_len) {
  jnx_char *decrypted_message = malloc(RSA_size(keypair));
  bzero(decrypted_message, RSA_size(keypair));
  char *err = malloc(120);

  if((*out_len = RSA_private_decrypt(in_len, message,
          decrypted_message, keypair, RSA_PKCS1_OAEP_PADDING)) == -1) {
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(),err);
    printf("%s\n",err);
    free(err);
    return NULL;
  }
  free(err);
  return decrypted_message;
}
jnx_char *symmetrical_encrypt(jnx_uint8 *key,jnx_uint8 *msg, jnx_size size){
#ifdef NOSYMMETRICALCRYPTO
  JNX_LOG(NULL,"---------------------NOT USING CRYPTOGRAPHY----------------");
  return strdup(msg);
#endif
  jnx_char *res;
  DES_cblock key2;
  DES_key_schedule schedule;
  jnx_int n = 0;
  size += 1;
  res = malloc(size);
  bzero(res,size);
  memcpy(key2,key,8);
  DES_set_odd_parity(&key2);
  DES_set_key_checked(&key2,&schedule);
  DES_cfb64_encrypt(msg,res,size,&schedule,&key2,&n,DES_ENCRYPT);
  return res;
}
jnx_char *symmetrical_decrypt(jnx_uint8 *key,jnx_uint8 *msg, jnx_size size){
#ifdef NOSYMMETRICALCRYPTO
  JNX_LOG(NULL,"---------------------NOT USING CRYPTOGRAPHY----------------");
  return strdup(msg);
#endif
  jnx_char *res;
  DES_cblock key2;
  jnx_int n = 0;
  DES_key_schedule schedule;
  size += 1;
  res = malloc(size);
  bzero(res,size);
  memcpy(key2,key,8);
  DES_set_odd_parity(&key2);
  DES_set_key_checked(&key2,&schedule);
  DES_cfb64_encrypt(msg,res,size,&schedule,&key2,&n,DES_DECRYPT);
  return res;
}
jnx_size generate_shared_secret(jnx_uint8 **buffer) {

  DES_cblock key;
  DES_cblock seed = { 0xFE, 0xFC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 };
  DES_key_schedule keysched;
  RAND_seed(seed,sizeof(DES_cblock));
  DES_random_key(&key);
  jnx_int i;
  jnx_int size = sizeof(DES_cblock);
  *buffer = calloc(size + 1,sizeof(jnx_uint8));
  bzero(*buffer,size + 1);
  memcpy(*buffer,key,size);
  return size;
} 

#pragma clang diagnostic pop
