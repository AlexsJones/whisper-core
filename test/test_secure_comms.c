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
#include "secure_comms.h"
#include "discovery.h"
static char *baddr = NULL;
static int linking_did_use_functor=0;
static int unlinking_did_use_functor=0;
int linking_test_procedure(session *s, void *optargs) {
  JNX_LOG(NULL,"Session hit linking procedure functor");
  linking_did_use_functor=1;
  return 0;
}
int unlinking_test_procedure(session *s, void *optargs) {
  JNX_LOG(NULL,"Session hit unlinking procedure functor");
  const char *arg = (const char*)optargs;
  JNXCHECK(strcmp(arg,"PASSED") == 0);
  unlinking_did_use_functor=1;
  return 0;
}
void test_secure_comms_receiver() {
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
 
  peer *l = peer_create(h,"127.0.0.1","Alex", 10);
  peer *n = peer_create(g,"127.0.0.1","Bob", 10);
  e = session_service_link_sessions(service,
      linking_test_procedure,
      NULL,
      &os->session_guid,l,n);
  JNXCHECK(e == SESSION_STATE_OKAY);
  JNXCHECK(linking_did_use_functor);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 1); 

  //Fake data required for the session
  os->is_connected = 1;
  os->secure_comms_port = "9015";

  peerstore *store = peerstore_init(l, 0);
  peerstore_store_peer(store,n);
  
  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  secure_comms_start(SC_RECEIVER,ds,os,AF_INET);

  e = session_service_unlink_sessions(service,
      unlinking_test_procedure,
      "PASSED",
      &os->session_guid);
  JNXCHECK(unlinking_did_use_functor);
  JNXCHECK(e == SESSION_STATE_OKAY);
  int r = session_service_session_is_linked(service,&os->session_guid);
  JNXCHECK(r == 0);
  session_service_destroy(&service);
  JNXCHECK(service == NULL);
}
int main(int argc, char **argv) {
  test_secure_comms_receiver();
  return 0;
}
