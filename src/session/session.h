/*
 * =====================================================================================
 *
 *       Filename:  session.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  25/01/2015 20:50:54
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef __SESSION_H__
#define __SESSION_H__
#include "cryptography.h"
#include <jnxc_headers/jnxguid.h>
#include <jnxc_headers/jnxqueue.h>
#include <jnxc_headers/jnxsocket.h>

typedef enum session_state {
  SESSION_STATE_OKAY,
  SESSION_STATE_FAIL,
  SESSION_STATE_NOT_FOUND,
  SESSION_STATE_EXISTS
}session_state;

typedef struct session {
  jnx_guid session_guid;
  /* discovery */
  jnx_guid local_peer_guid;
  jnx_guid remote_peer_guid;
  /* crypto */
  jnx_char *initiator_public_key;
  jnx_char *receiver_public_key;
  jnx_uint8 *shared_secret;
  /* secure network */
  jnx_int is_connected;
  jnx_char *secure_comms_port;
  jnx_int secure_socket;

  /* gui */
  void *gui_context;
  /* local only */
  RSA *keypair;
}session;

void default_session_callback(void *gui_context, jnx_guid *session_guid,
    jnx_char *decrypted_message);

session_state session_message_write(session *s,jnx_uint8 *message);

jnx_int session_message_read(session *s, jnx_uint8 **omessage);

session_state session_message_read_and_decrypt(session *s, jnx_uint8 *message,
    jnx_uint8 **omessage);

jnx_int session_is_active(session *s);

void session_add_initiator_public_key(session *s, jnx_char *key);

void session_add_receiver_public_key(session *s, jnx_char *key);

void session_add_shared_secret(session *s, jnx_uint8 *secret);

void session_add_secure_comms_port(session *s, jnx_char *comms_port);

void session_add_remote_peer_guid(session *s, jnx_uint8 *guid_str);
#endif
