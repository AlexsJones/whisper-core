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
#include <jnxc_headers/jnx_guid.h>
#include <jnxc_headers/jnx_queue.h>
#include <jnxc_headers/jnx_socket.h>
#include <jnxc_headers/jnx_list.h>
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
  jnx_int secure_receiver_listen_socket;

  /* gui */
  void *gui_context;
  jnx_uint8 *initiator_message;
  /* local only */
  RSA *keypair;
  /* foriegn sessions that are shared with this session */
  jnx_list *foriegn_sessions;
}session;

typedef struct foriegn_session_message {
  jnx_guid session_guid;
  jnx_uint8 *message;
}foriegn_session_message;
/*
 *@fn session_state session_message_write(sesssion *s, jnx_uint8 *message)
 *@brief writes a message that goes through the encryption layer to a remote session peer (if active)
 *@param s is the session pointer
 *@param message is the message to send
 *@return session_state should be SESSION_STATE_OKAY on success
 */
session_state session_message_write(session *s,jnx_uint8 *message);
/*
 *@fn jnx_int session_message_read(session *s, jnx_uint8 **omessage);
 *@brief blocks until it has read data from the secure_socket
 *@param s is the session pointer
 *@param omessage is a double pointer that mallocs and populates with buffer data (NULL if empty)
 *@return bytes read (-1 on failure)
 */
jnx_int session_message_read(session *s, jnx_uint8 **omessage);



jnx_int session_message_read_foreign_sessions(session *s, jnx_list **omessagelist);
/*
 *@fn jnx_int session_is_active(session *s)
 *@brief this function is a quick way of determining if you can write to the remote session socket, usually polled after linking
 *@param s is the session pointer
 *@return 1 on active 0 on inactive
 */
jnx_int session_is_active(session *s);
#endif
