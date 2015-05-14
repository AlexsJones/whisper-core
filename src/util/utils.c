/*
 * utils.c
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */

#include "utils.h"

void print_guid(jnx_guid *g) {
  jnx_char *str;
  jnx_guid_to_string(g,&str);
  JNXLOG(LDEBUG,"%s",str);
  free(str);
}
void print_pair(jnx_guid *a, jnx_guid *b) {
  jnx_char *str,*str2;
  jnx_guid_to_string(a,&str);
  jnx_guid_to_string(b,&str2);
  JNXLOG(LDEBUG,"[%s] [%s]",str,str2);
  free(str);
  free(str2);
}
void print_peer(peer *p) {
  printf("----------\n");
  jnx_char *str;
  jnx_guid_to_string(&(*p).guid,&str);
  printf("Peer guid => %s\n",str);
  free(str);
  printf("Peer host_addres => %s\n",p->host_address);
  printf("Peer user_name => %s\n",p->user_name);
  printf("Peer last seen => %ld\n",p->last_seen);
  printf("----------\n");
}
void print_public_key(RSA *remote_pub_keypair) {

  jnx_char *s = asymmetrical_key_to_string(remote_pub_keypair,
    PUBLIC);
  printf("[%s]\n",s);
  free(s);
}