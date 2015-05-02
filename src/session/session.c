/*
 * session.c
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */
#include <sys/socket.h>
#include "session.h"
#include "secure_comms.h"

void default_session_callback(void *gui_context, jnx_guid *session_guid,
    jnx_char *decrypted_message) {

  printf("default_session_callback: %s.\n",decrypted_message);
  free(decrypted_message);
}
session_state session_message_write(session *s,jnx_char *message) {
  /* take the raw message and des encrypt it */
  jnx_size len = strlen(message);
  jnx_char *encrypted = symmetrical_encrypt((jnx_uint8*)s->shared_secret,
      (jnx_uint8*)message,
      len);

  int send_result = 0;
  if (0 > (send_result = send(s->secure_socket,encrypted,strlen(encrypted),0))) {
    session_disconnect(s);
    perror("session: write");
    return SESSION_STATE_FAIL;
  }
  JNX_LOG(0,"Send result => %d\n",send_result);
  return SESSION_STATE_OKAY;
}
jnx_int session_message_read(session *s, jnx_char **omessage) {
  *omessage = NULL;
  if(!s->is_connected) {
    JNX_LOG(0,"Session not connected, cannot read");
    return -1;
  }
  if(s->secure_socket == -1 ) {
    JNX_LOG(0,"Session cannot read from a null socket");
    return -1;
  }
  jnx_char buffer[2048];
  bzero(buffer,2048);
  jnx_int bytes_read = recv(s->secure_socket,buffer,2048,0);
  if(bytes_read > 0) {
    jnx_char *decrypted_message = symmetrical_decrypt(s->shared_secret,
        buffer,bytes_read);
    *omessage = decrypted_message;

    printf("%s\n",buffer);
  }
  return bytes_read;
}
session_state session_message_read_and_decrypt(session *s, 
    jnx_char *message,jnx_char **omessage) {
  jnx_size len = strlen(message);
  jnx_char *decrypted = symmetrical_decrypt((jnx_uint8*)s->shared_secret,
      (jnx_uint8*)message,len);
  *omessage = decrypted;
  return SESSION_STATE_OKAY;
}
session_state session_disconnect(session *s) {
  if (s->is_connected) {
    if(s->secure_socket) {
      secure_comms_end(s);
    }
  }
  s->is_connected = 0;
  return SESSION_STATE_OKAY;
}
void session_add_initiator_public_key(session *s, jnx_char *key) {
  JNXCHECK(key);
  jnx_size len = strlen(key);
  s->initiator_public_key = malloc(len * sizeof(jnx_char));
  memcpy(s->initiator_public_key,key,len);
}
void session_add_receiver_public_key(session *s, jnx_char *key) {
  JNXCHECK(key);
  jnx_size len = strlen(key);
  s->receiver_public_key = malloc(len * sizeof(jnx_char));
  memcpy(s->receiver_public_key,key,len);
}
void session_add_shared_secret(session *s, jnx_char *secret) {
  JNXCHECK(secret);
  jnx_size len = strlen(secret);
  s->shared_secret = malloc(len * sizeof(jnx_char));
  memcpy(s->shared_secret,secret,len);
}
void session_add_secure_comms_port(session *s, jnx_char *comms_port) {
  JNXCHECK(comms_port);
  jnx_size len = strlen(comms_port) +1;
  s->secure_comms_port = malloc(len * sizeof(jnx_char));
  bzero(s->secure_comms_port,len);
  memcpy(s->secure_comms_port,comms_port,len);
}
void session_add_remote_peer_guid(session *s, jnx_uint8 *guid_str) {
  JNXCHECK(guid_str);
  jnx_guid g;
  jnx_guid_from_string((jnx_char*)guid_str,&g);
  s->remote_peer_guid = g;
}
