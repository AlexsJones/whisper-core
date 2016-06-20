/*********************************************************************************
*     File Name           :     src/session/session.h
*     Created By          :     jonesax
*     Creation Date       :     [2016-06-19 16:58]
*     Last Modified       :     [2016-06-20 10:57]
*     Description         :      
**********************************************************************************/

#ifndef __SESSION_H__
#define __SESSION_H__
#include "peer.h"
#include "connection_request.h"
#include <jnxc_headers/jnx_guid.h>
typedef struct session {
  jnx_guid id;
  jnx_list *connection_request_list;
}session;

session *session_create();

void session_add_connection(session *s, connection_request *c);

void session_destroy(session **s);
#endif
