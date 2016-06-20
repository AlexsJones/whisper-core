/*********************************************************************************
 *     File Name           :     /home/jonesax/Work/whisper-core/src/session/session_controller.c
 *     Created By          :     jonesax
 *     Creation Date       :     [2016-06-19 17:30]
 *     Last Modified       :     [2016-06-20 14:54]
 *     Description         :      
 **********************************************************************************/
#include "session_controller.h"
#include "connection_controller.h"

static session_controller *scontroller_handle = NULL;

void internal_incoming(const connection_request *c) { 
  JNXLOG(LDEBUG,"Session controller internal incoming");
  //Create a session based on the incoming connection
  JNXCHECK(scontroller_handle);
  connection_request *connection = (connection_request*)c;
  session *s = session_controller_session_create_from_incoming((session_controller*)
      scontroller_handle,
      (connection_request*)connection);

  session_controller_add_session(scontroller_handle,s);
}
void internal_completed(const connection_request *c) {
  JNXLOG(LDEBUG,"Session controller internal completed");
}
void internal_closed(const connection_request *c) {
  JNXLOG(LDEBUG,"Session controller internal closed");
}
session_controller *session_controller_create(
    connection_controller *connection_controller) {
  session_controller *sc = malloc(sizeof(session_controller));
  sc->session_list = jnx_list_create();
  sc->connection_controller = connection_controller;
  sc->connection_controller->nc = internal_completed;
  sc->connection_controller->ic = internal_incoming;
  sc->connection_controller->cc = internal_closed;
  
  scontroller_handle = sc;
  return sc;
}
E_SC_STATE session_controller_add_session(session_controller *sc, session *s) {
  jnx_list_add(sc->session_list,s);
}
int compare_sessions(void *A, void *B) {

  session *sa = (session*)A;
  session *sb = (session*)B;

  if(jnx_guid_compare(&(*sa).id,&(*sb).id) == JNX_GUID_STATE_SUCCESS) {
    return 0;
  }

  return 1;
}
E_SC_STATE session_controller_remove_session(session_controller *sc, session *s) {
  
  jnx_list_remove_from(&(*sc).session_list,s,compare_sessions);
}
session *session_controller_session_create(session_controller *s,
    peer *remote_peer) {
  JNXLOG(LDEBUG,"------------------------------------------------------------");
  session *ses = session_create();
  //Add the peer as a connection
  connection_request *request;
  connection_controller_state e = connection_controller_initiation_request(
      s->connection_controller,remote_peer,
      &request);
  JNXCHECK(e == E_CCS_OKAY);
  JNXCHECK(request);
  session_add_connection(ses,request);
 
  jnx_char *sguid, *rguid;
  jnx_guid_to_string(&(*ses).id,&sguid);
  jnx_guid_to_string(&(*request).id,&rguid);
  JNXLOG(LDEBUG,"--------------Created new session %s with connection request %s",
      sguid,rguid);
  free(sguid);
  free(rguid);
  JNXLOG(LDEBUG,"------------------------------------------------------------");
  session_controller_add_session(s,ses);
  return ses;
}
session *session_controller_session_create_from_incoming(session_controller *s,
    connection_request *req) {

  JNXLOG(LDEBUG,"------------------------------------------------------------");
  session *ses = session_create();
  session_add_connection(ses,req);

  jnx_char *sguid, *rguid;
  jnx_guid_to_string(&(*ses).id,&sguid);
  jnx_guid_to_string(&(*req).id,&rguid);
  JNXLOG(LDEBUG,"--------------Created new response session %s with connection request %s",
      sguid,rguid);
  free(sguid);
  free(rguid);
  JNXLOG(LDEBUG,"------------------------------------------------------------");
  JNXLOG(LDEBUG,"------------------------------------------------------------");
  return ses;
}
void session_controller_session_add_peer(session_controller *sc,
    session *s, peer *remote_peer) {

  connection_request *request;
  connection_controller_state e = connection_controller_initiation_request(
      sc->connection_controller,remote_peer,
      &request);

  session_add_connection(s,request);
}
int session_controller_is_session_ready(session_controller *sc,
    session *s) {

  jnx_list *connection_list = s->connection_request_list;

  jnx_node *head = connection_list->head;
  while(head) {

    connection_request *r = head->_data;
    jnx_char *guid;
    jnx_guid_to_string(&(*r).id,&guid);
    if(r->state != E_CRS_COMPLETE) {
      JNXLOG(LDEBUG,"Session found connection request %s that is not ready",guid);
      free(guid);
      return 0;
    } 
    free(guid);
    head = head->next_node;
  }
  JNXLOG(LDEBUG,"Session found that all of its connections are ready!");
  return 1;
}
void session_controller_session_message_send(session_controller *sc,
    session *s) {


}
void session_controller_destroy(session_controller **sc) {

  jnx_node *h = (*sc)->session_list->head;

  while(h) {
    session *s = h->_data;
    session_destroy(&s); 
    h = h->next_node;
  }

  jnx_list_destroy(&(*sc)->session_list);
  free(*sc);
  *sc = NULL;
  scontroller_handle = NULL;
}
