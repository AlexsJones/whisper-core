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
#include <jnxc_headers/jnxcheck.h>
#include <jnxc_headers/jnxlog.h>

void test_rsa_encryption() {

  RSA *key = asymmetrical_generate_key(2048);
  jnx_char *test_message = "Test";
  jnx_int s = strlen(test_message); 

  jnx_size olen,second_olen;
  jnx_char *encrypted_message = asymmetrical_encrypt(key,test_message,&olen);

  JNX_LOG(NULL,"Encrypted RSA length %d >%s<\n",olen,encrypted_message);

  jnx_char *decrypted_message = asymmetrical_decrypt(key,encrypted_message,olen,&second_olen);

  JNX_LOG(NULL,"Decrypted message %d >%s<\n",second_olen,decrypted_message);

  JNXCHECK(strcmp(decrypted_message,test_message) == 0);
}
void test_rsa_key() {
  RSA *key = asymmetrical_generate_key(2048);
  JNXCHECK(key);

  jnx_char *keystring = asymmetrical_key_to_string(key,PUBLIC);

  JNX_LOG(NULL,"Key \n%s",keystring);
  JNXCHECK(keystring);  
  jnx_char *keystringprivate = asymmetrical_key_to_string(key,PRIVATE);

  JNX_LOG(NULL,"Key \n%s",keystringprivate);
  JNXCHECK(keystringprivate);  

  free(keystring);
  free(keystringprivate);
  asymmetrical_destroy_key(key);

}
void test_des_encryption() {
  jnx_char *test_message = "Test";
  jnx_size message_size = strlen(test_message);

  jnx_char *encrypted_message = symmetrical_encrypt("TestKey",test_message,message_size);

  jnx_char *decrypted_message = symmetrical_decrypt("TestKey",encrypted_message,message_size);

  JNXCHECK(strcmp(decrypted_message,test_message) == 0);

  free(encrypted_message);
  free(decrypted_message);
}
int main(int argc, char **argv) {
  JNX_LOG(NULL,"Test RSA key");
  test_rsa_key();
  JNX_LOG(NULL,"Test RSA encryption");
  test_rsa_encryption();
  JNX_LOG(NULL,"Test DES encryption");
  test_des_encryption();
  return 0;
}
