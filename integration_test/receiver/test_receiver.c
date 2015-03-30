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
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include "session_service.h"
#include "auth_comms.h"
#include "discovery.h"
static char *baddr = NULL;


int linking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {
  JNX_LOG(NULL,"Linking now the receiver session..");
/*
  jnx_char *default_secure_comms = "6666";
  auth_comms_service *ac = auth_comms_create();
  ac->listener = jnx_socket_tcp_listener_create("9991",AF_INET,15);
  auth_comms_initiator_start(ac,ds,os,default_secure_comms);
*/
  return 0;
}
int unlinking_test_procedure(session *s,linked_session_type session_type, 
    void *optargs) {
  return 0;
}
void test_receiver() {
  JNX_LOG(NULL,"test_linking");
  session_service *service = session_service_create(linking_test_procedure,
      unlinking_test_procedure);
  session *os;
  session_state e = session_service_create_session(service,&os);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 0);
  //Lets generate the guid of some remote session
  jnx_guid h;
  jnx_guid_create(&h);

  peerstore *store = peerstore_init(local_peer_for_user("initiator_bob",10), 0);

  get_broadcast_ip(&baddr);

  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  discovery_service_start(ds,BROADCAST_UPDATE_STRATEGY);

  auth_comms_service *ac = auth_comms_create();
  ac->listener = jnx_socket_tcp_listener_create("9991",AF_INET,15);
  auth_comms_listener_start(ac,ds,service,NULL);
 
  while(!os->is_connected) {
    JNX_LOG("Awaiting connection completion");
    sleep(.5);
  }
}
int main(int argc, char **argv) {
  test_receiver();
  return 0;
}
