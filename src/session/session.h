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

typedef struct session {
  jnx_guid session_guid;
  /* discovery --> peer reference via guid */
  jnx_guid origin_peer_guid;


}session;


void session_message_write(session *s,jnx_uint8 *message);

jnx_int session_message_read(session *s, jnx_uint8 **omessage);

#endif
