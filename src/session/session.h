/*********************************************************************************
*     File Name           :     src/session/session.h
*     Created By          :     jonesax
*     Creation Date       :     [2016-06-19 16:58]
*     Last Modified       :     [2016-06-19 17:03]
*     Description         :      
**********************************************************************************/

#ifndef __SESSION_H__
#define __SESSION_H__
#include "peer.h"
#include "connection_controller.h"
typedef struct session {
  const connection_controller *controller;
}session;


session *session_create(peer *remote,connection_controller *c);

void session_destroy(session **s);
#endif
