/*********************************************************************************
*     File Name           :     /home/jonesax/Work/whisper-core/src/session/session.c
*     Created By          :     jonesax
*     Creation Date       :     [2016-06-19 17:03]
*     Last Modified       :     [2016-06-21 08:30]
*     Description         :      
**********************************************************************************/

#include "session.h"

session *session_create() {
  session *s = malloc(sizeof(session));
  s->connection_request_list = jnx_list_create();
  jnx_guid_create(&(*s).id); 
  return s;
}
void session_add_connection(session *s, connection_request *c) {
  jnx_list_add(s->connection_request_list,c);
}
int compare_connections(void *a, void *b) {
  connection_request *ac = (connection_request*)a;
  connection_request *bc = (connection_request*)b;
  if(jnx_guid_compare(&(*ac).id,&(*bc).id) == JNX_GUID_STATE_SUCCESS) {
    return 0;
  }
  return 1;
}
void session_remove_connection(session *s, connection_request *c) {
  
  jnx_list_remove_from(&(*s).connection_request_list,c,compare_connections);
}
void session_destroy(session **s) {
  jnx_char *sid;
  jnx_guid_to_string(&(*s)->id,&sid);
  JNXLOG(LDEBUG,"Session %s destroyed",sid);
  free(sid);
  jnx_list_destroy(&(*s)->connection_request_list);
  free(*s);
  *s = NULL;
}
