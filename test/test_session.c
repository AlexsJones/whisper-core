/*
 * =====================================================================================
 *
 *       Filename:  test_session.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2015 05:57:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "session_service.h"
void test_create_destroy() {
  JNX_LOG(NULL,"test_create_destroy");
  session_service *service = session_service_create();
  session *os;
  //Create a session
  session_state e = session_service_create_session(service,&os);
  JNXCHECK(e == SESSION_STATE_OKAY);
  JNXCHECK(service->session_list->counter == 1);
  //Pulling out that session guid lets get a handle on our session
  session *retrieved = NULL;
  //Lets test we can retrieve our session by giving our service the guid
  e = session_service_fetch_session(service,&os->session_guid,&retrieved);
  JNXCHECK(e == SESSION_STATE_OKAY);
  JNXCHECK(jnx_guid_compare(&os->session_guid,&retrieved->session_guid) == JNX_GUID_STATE_SUCCESS);
  e = session_service_destroy_session(service,&os->session_guid);
  JNXCHECK(e == SESSION_STATE_OKAY);
  JNXCHECK(service->session_list->counter == 0);
  session_service_destroy(&service);
  JNXCHECK(service == NULL);
}
static int linking_did_use_functor=0;
int linking_test_procedure(session *s, void *optargs) {
  JNX_LOG(NULL,"Session hit linking procedure functor");
  linking_did_use_functor=1;
  return 0;
}
void test_linking() {
  JNX_LOG(NULL,"test_linking");
  session_service *service = session_service_create();
  session *os;
  session_state e = session_service_create_session(service,&os);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 0); 
  //Lets generate the guid of some remote session
  jnx_guid h;
  jnx_guid g;
  jnx_guid_create(&h);
  jnx_guid_create(&g);
 
  peer *l = peer_create(h,"N/A","Bob", 10);
  peer *n = peer_create(g,"N/A","Bob", 10);
  e = session_service_link_sessions(service,
      linking_test_procedure,
      NULL,
      &os->session_guid,l,n);
  JNXCHECK(e == SESSION_STATE_OKAY);
  JNXCHECK(linking_did_use_functor);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 1); 
  e = session_service_unlink_sessions(service,&os->session_guid);
  JNXCHECK(e == SESSION_STATE_OKAY);
  int r = session_service_session_is_linked(service,&os->session_guid);
  JNXCHECK(r == 0);
  session_service_destroy(&service);
  JNXCHECK(service == NULL);
}
int main(int argc, char **argv) {
  test_create_destroy();
  test_linking();
  return 0;
}
