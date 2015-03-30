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
#include "secure_comms.h"
#include "discovery.h"
static char *baddr = NULL;

int linking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {
  JNX_LOG(NULL,"Session hit linking procedure functor");
  return 0;
}
int unlinking_test_procedure(session *s,linked_session_type session_type, 
    void *optargs) {
  return 0;
}
void test_initiator() {
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

  int remote_peers = 0;
  jnx_guid **active_guids;
  peer *local = peerstore_get_local_peer(store);
  peer *remote_peer = NULL;
  while(!remote_peers) {
    int num_guids = peerstore_get_active_guids(store,&active_guids);
    int i;
    for(i=0;i<num_guids;i++) {
      jnx_guid *guid = active_guids[i];
      peer *p = peerstore_lookup(store,guid);
      if(peers_compare(p,local) != 0) {
        printf("Found a remote peer! Breaking!\n");
        remote_peers = 1;
        remote_peer = p;
        break;
      }
    }
  }


}
int main(int argc, char **argv) {
  test_initiator();
  return 0;
}
