/*********************************************************************************
 *     File Name           :     src/session/session_controller.h
 *     Created By          :     jonesax
 *     Creation Date       :     [2016-06-19 17:21]
 *     Last Modified       :     [2016-07-28 11:57]
 *     Description         :      
 **********************************************************************************/
#ifndef __SESSION_CONTROLLER_H__
#define __SESSION_CONTROLLER_H__
#include "connection_controller.h"
#include "session.h"
#include <jnxc_headers/jnx_list.h>

void typedef (*user_session_message_notification)(const session *s, 
    const connection_request *c, const jnx_char *message,
    jnx_size message_len);

typedef struct session_controller {
  connection_controller *connection_controller;
  jnx_list *session_list; 
  const discovery_service *discovery;
  user_session_message_notification umn;
}session_controller;

session_controller *session_controller_create( 
    connection_controller *connection_controller, user_session_message_notification n);

session *session_controller_session_create(session_controller *s,
    peer *remote_peer);

typedef enum E_SC_STATE{ 
  E_SC_OKAY,
  E_SC_FAILED
}E_SC_STATE;

E_SC_STATE session_controller_add_session(session_controller *sc, session *s);

E_SC_STATE session_controller_remove_session(session_controller *sc, session *s);

session *session_controller_session_create_from_incoming(session_controller *s,
    connection_request *req);

void session_controller_session_add_peer(session_controller *sc,
    session *s, peer *remote_peer);

int session_controller_is_session_ready(session_controller *sc,
    session *s);

void session_controller_destroy(session_controller **sc);

void session_controller_session_send_message(session_controller *sc, session *s, 
  jnx_char *message, jnx_size message_len);
#endif

