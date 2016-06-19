/*********************************************************************************
*     File Name           :     /home/jonesax/Work/whisper-core/src/session/session.c
*     Created By          :     jonesax
*     Creation Date       :     [2016-06-19 17:03]
*     Last Modified       :     [2016-06-19 17:04]
*     Description         :      
**********************************************************************************/

#include "session.h"

session *session_create(peer *remote,connection_controller *c) {
  session *s = malloc(sizeof(session));
  s->controller = c;
  return s;
}
void session_destroy(session **s) {
 
  *s = NULL;
  free(*s);
}
