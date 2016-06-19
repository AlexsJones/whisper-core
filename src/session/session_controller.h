/*********************************************************************************
*     File Name           :     src/session/session_controller.h
*     Created By          :     jonesax
*     Creation Date       :     [2016-06-19 17:21]
*     Last Modified       :     [2016-06-19 18:08]
*     Description         :      
**********************************************************************************/
#ifndef __SESSION_CONTROLLER_H__
#define __SESSION_CONTROLLER_H__
#include "connection_controller.h"
#include "session.h"
#include <jnxc_headers/jnx_list.h>
typedef struct session_controller {
  connection_controller *connection_controller;
  jnx_list *session_list; 
}session_controller;

session_controller *session_controller_create(
    connection_controller *connection_controller);

session *session_controller_session_create(session_controller *s,
    peer *remote_peer);

void session_controller_session_add_peer(session_controller *sc,
    session *s, peer *remote_peer);

int session_controller_is_session_ready(session_controller *sc,
    session *s);

void session_controller_destroy(session_controller **sc);
#endif

