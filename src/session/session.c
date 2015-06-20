/*
 * session.c
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */
#include <sys/socket.h>
#include "session.h"
#include "secure_comms.h"

jnx_int session_is_active(session *s) {
  jnx_int is_socket_live = secure_comms_is_socket_linked(s->secure_socket);
  if(s->is_connected && is_socket_live) {
    return 1;
  }
  return 0;
}
session_state session_message_write(session *s,jnx_uint8 *message) {
  /* take the raw message and des encrypt it */
  jnx_size len = strlen(message);

  jnx_char *encrypted = symmetrical_encrypt(s->shared_secret,
      message,
      len);

  int send_result = 0;
  if (0 > (send_result = send(s->secure_socket,encrypted,strlen(encrypted),0))) {
    perror("send:");
    return SESSION_STATE_FAIL;
  }
  JNXLOG(LDEBUG,"Send result => %d\n",send_result);

  /* foriegn_session replication */
  if(s->foriegn_sessions) {
    jnx_node *head = s->foriegn_sessions->head,
             *reset = s->foriegn_sessions->head;
    while(head) {
      session *current_foriegn_session = head->_data;
      encrypted = symmetrical_encrypt(current_foriegn_session->shared_secret,
          message,len);

      if (0 > (send_result = send(current_foriegn_session->secure_socket,
              encrypted,strlen(encrypted),0))) {
        perror("send:");
        JNXLOG(LWARN,"Failure to send on foriegn session");
        head = head->next_node;
        return SESSION_STATE_FAIL;
      }
      JNXLOG(LDEBUG,"foriegn_session send result => %d\n",send_result);
      free(encrypted);
      head = head->next_node;
    }
    head = reset;
  } 
  /* foriegn_session replication */
  return SESSION_STATE_OKAY;
}
jnx_int session_message_read(session *s, jnx_uint8 **omessage) {
  *omessage = NULL;
  if(!s->is_connected) {
    JNXLOG(LDEBUG,"Session not connected, cannot read");
    return -1;
  }
  if(s->secure_socket == -1 ) {
    JNXLOG(LDEBUG,"Session cannot read from a null socket");
    return -1;
  }
  jnx_uint8 buffer[2048];
  bzero(buffer,2048);
  jnx_int bytes_read = recv(s->secure_socket,buffer,2048,0);
  if(bytes_read > 0) {
    jnx_char *decrypted_message = symmetrical_decrypt(s->shared_secret,
        buffer,bytes_read);
    *omessage = decrypted_message;
  }
  return bytes_read;
}
jnx_int session_message_read_foreign_sessions(session *s, 
    jnx_list **omessagelist) {
  int message_count = 0;
  
  if(s->foriegn_sessions) {
    jnx_node *head = s->foriegn_sessions->head,
             *reset = s->foriegn_sessions->head;
    while(head) {
      session *current_foriegn_session = head->_data;
      jnx_uint8 *message;
      jnx_int bytes_read = session_message_read(current_foriegn_session,&message);
      if(bytes_read){
      
        foriegn_session_message *fsm = malloc(sizeof(foriegn_session_message));
        fsm->message = message;
        fsm->session_guid = current_foriegn_session->session_guid;
        jnx_list_add(*omessagelist,fsm);
        ++message_count;
      }
    }
    head = reset;
  }
  return message_count;
}
session_state session_message_read_and_decrypt(session *s, 
    jnx_uint8 *message,jnx_uint8 **omessage) {
  jnx_size len = strlen(message);
  jnx_char *decrypted = symmetrical_decrypt(s->shared_secret,
      message,len);
  *omessage = decrypted;
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
void session_add_shared_secret(session *s, jnx_uint8 *secret) {
  JNXCHECK(secret);
  jnx_size len = strlen(secret) + 1;
  s->shared_secret = malloc(len * sizeof(jnx_uint8));
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
void session_add_initiator_message(session *s, jnx_uint8 *message) {
  JNXCHECK(message);
  jnx_size len = strlen(message) +1;
  s->initiator_message = malloc(len * sizeof(jnx_char));
  bzero(s->initiator_message,len);
  memcpy(s->initiator_message,message,len);
}
void session_add_foreign_session(session *s, session *foriegn_session){
  if(s->foriegn_sessions == NULL) {
    s->foriegn_sessions = jnx_list_create();
  }
  jnx_list_add_ts(s->foriegn_sessions,foriegn_session);
}
void session_remove_foreign_sessions(session *s) {
  if(s->foriegn_sessions == NULL) {
    JNXLOG(LWARN,"session_remove_foreign_sessions: Nothing to remove");
    return;
  }
  jnx_list_destroy(&s->foriegn_sessions);
}
