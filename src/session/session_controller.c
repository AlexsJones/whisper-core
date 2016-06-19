/*********************************************************************************
 *     File Name           :     /home/jonesax/Work/whisper-core/src/session/session_controller.c
 *     Created By          :     jonesax
 *     Creation Date       :     [2016-06-19 17:30]
 *     Last Modified       :     [2016-06-19 19:33]
 *     Description         :      
 **********************************************************************************/
#include "session_controller.h"
#include "connection_controller.h"

void internal_incoming(const connection_request *c) { 
  JNXLOG(LDEBUG,"Session controller internal incoming");
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
  return sc;
}
session *session_controller_session_create(session_controller *s,
    peer *remote_peer) {

  session *ses = session_create();
  //Add the peer as a connection
  connection_request *request;
  connection_controller_state e = connection_controller_initiation_request(
      s->connection_controller,remote_peer,
      &request);

  session_add_connection(ses,request);

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
    if(r->state != E_CRS_COMPLETE) {
      JNXLOG(LDEBUG,"Session found connection request that is not ready");
      return 0;
    } 
    head = head->next_node;
  }
  JNXLOG(LDEBUG,"Session found that all of its connections are ready!");
  return 1;
}
void session_controller_destroy(session_controller **sc) {

  jnx_list_destroy(&(*sc)->session_list);
  free(*sc);
  *sc = NULL;
}
