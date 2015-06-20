/*
 * session_service_auth_comms.h
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef HANDSHAKE_CONTROL_H
#define HANDSHAKE_CONTROL_H
#include "session.h"
#include "discovery.h"
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxthread.h>

typedef enum {
  CHALLENGE_FAIL,
  CHALLENGE_PUBLIC_KEY,
  CHALLENGE_FINISH
}handshake_initiator_state;

typedef enum {
  JOINER_JOIN
}handshake_joiner_state;

typedef enum { 
  REPONSE_FAIL,
  RESPONSE_PUBLIC_KEY,
  RESPONSE_FINISH
}handshake_receiver_state;

int handshake_did_receive_initiator_request(jnx_uint8 *obuffer,
    jnx_size bytes_read,void **oobject);

int handshake_did_receive_receiver_request(jnx_uint8 *obuffer,
    jnx_size bytes_read,void **oobject);
/* Initiator request */
int handshake_initiator_command_generate(session *ses,\
    handshake_initiator_state state, jnx_uint8 *shared_secret,jnx_size len,
    jnx_uint8 *initiator_message,
    jnx_uint8 **onetbuffer);

int handshake_generate_public_key_request(session *ses,\
    jnx_uint8 *initiator_message,
    jnx_uint8 **onetbuffer);

int handshake_generate_finish_request(session *ses,\
    jnx_uint8 *shared_secret,jnx_size secret_len,
    jnx_uint8 **onetbuffer);
/* Receiver request */
int handshake_receiver_command_generate(session *ses, \
    handshake_receiver_state state, jnx_int abort, jnx_uint8 **onetbuffer);

int handshake_generate_public_key_response(session *ses,\
    jnx_int abort,
    jnx_uint8 **onetbuffer);

int handshake_generate_finish_response(session *ses,\
    jnx_int abort,
    jnx_uint8 **onetbuffer);
/* Joiner request */
int handshake_joiner_command_generate(session *ses, \
    handshake_joiner_state state, jnx_guid *session_guid,\
    jnx_uint8 **onetbuffer);
#endif
