/*
 * =====================================================================================
 *
 *       Filename:  test_crypto.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19/01/15 11:20:13
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
#include <jnxc_headers/jnxencoder.h>
#include <jnxc_headers/jnxcheck.h>
#include <jnxc_headers/jnxlog.h>

void test_secret_generate() {
  jnx_uint8 *buffer;

  jnx_int len = generate_shared_secret(&buffer);

  JNXCHECK(len == 8); 
}
void test_rsa_encryption() {

  RSA *key = asymmetrical_generate_key(2048);
  jnx_char *test_message = "Test";
  jnx_int s = strlen(test_message); 

  jnx_size olen,second_olen;
  jnx_char *encrypted_message = asymmetrical_encrypt(key,test_message,&olen);

  jnx_char *decrypted_message = asymmetrical_decrypt(key,encrypted_message,olen,&second_olen);

  JNXCHECK(strcmp(decrypted_message,test_message) == 0);
}
void test_rsa_key() {
  RSA *key = asymmetrical_generate_key(2048);
  JNXCHECK(key);

  jnx_char *keystring = asymmetrical_key_to_string(key,PUBLIC);

  JNXLOG(LDEBUG,"Key \n%s",keystring);
  JNXCHECK(keystring);  
  jnx_char *keystringprivate = asymmetrical_key_to_string(key,PRIVATE);

  JNXLOG(LDEBUG,"Key \n%s",keystringprivate);
  JNXCHECK(keystringprivate);  

  free(keystring);
  free(keystringprivate);
  asymmetrical_destroy_key(key);

}
void test_des_encryption() {
  jnx_char *test_message = "Test";
  jnx_size message_size = strlen(test_message);

  jnx_uint8 *buffer;

  jnx_int len = generate_shared_secret(&buffer);

  jnx_char *encrypted_message = symmetrical_encrypt(buffer,test_message,message_size);

  jnx_char *decrypted_message = symmetrical_decrypt(buffer,encrypted_message,message_size);

  JNXCHECK(strcmp(decrypted_message,test_message) == 0);

  JNXLOG(LDEBUG,"DES decrypted_message => %s",decrypted_message);

  free(encrypted_message);
  free(decrypted_message);
  free(buffer);

}
static void print_hex(jnx_int size, jnx_char *ar) {

  jnx_int i;
  for(i=0;i<size; ++i) {
    printf("%02X",ar[i]);
  }
  printf("\n");
}
void test_multilevel_encryption() {

  const jnx_char *string ="Test string";
  jnx_size string_size = strlen(string);

  const jnx_uint8 *sbuffer = "ABCDEFGH";
  jnx_size s = strlen(sbuffer);
  jnx_encoder *e = jnx_encoder_create();

  jnx_char *encrypted_string = symmetrical_encrypt(sbuffer,
      string,string_size);

  RSA *keypair = asymmetrical_generate_key(2048);

  jnx_size encrypted_olen;

  jnx_char *encrypted_message = asymmetrical_encrypt(keypair,encrypted_string,
      &encrypted_olen);

  /*decrypted_message*/
  jnx_size decrypted_olen;
  jnx_char *decrypted_message = asymmetrical_decrypt(keypair,encrypted_message,
      encrypted_olen,&decrypted_olen);

  jnx_char *decrypted_symmetrical_message =symmetrical_decrypt(sbuffer,decrypted_message,
      decrypted_olen);

  JNXCHECK(strcmp(decrypted_symmetrical_message,string) == 0);

  free(encrypted_message);
  free(decrypted_message);
  asymmetrical_destroy_key(keypair);
}
int main(int argc, char **argv) {
  JNXLOG(LDEBUG,"Test generate shared secret");
  test_secret_generate();
  JNXLOG(LDEBUG,"Test RSA key");
  test_rsa_key();
  JNXLOG(LDEBUG,"Test RSA encryption");
  test_rsa_encryption();
  JNXLOG(LDEBUG,"Test DES encryption");
  test_des_encryption();
  JNXLOG(LDEBUG,"Test multilevel encryption");
  int i;
  for(i=0;i<150;++i){
    test_multilevel_encryption();
  }
  sleep(1);
  return 0;
}
