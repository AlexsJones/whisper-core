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
#include <jnxc_headers/jnxthread.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include "session_service.h"
#include "port_control.h"
#include "auth_comms.h"
#include "discovery.h"
static char *baddr = NULL;
static char *interface = NULL;
static auth_comms_service *ac = NULL;
int linking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {
  if(session_type == E_AM_INITIATOR){
    JNXCHECK(session_type == E_AM_INITIATOR);
    JNXLOG(NULL,"Session hit linking procedure functor");
    discovery_service *ds = (discovery_service*)optargs;
    /* Adding port control service */

    int init_port = rand() % 1000;

    port_control_service *ps = port_control_service_create(8000 + 
        init_port,
        12341,1);

    ac = auth_comms_create();

    ac->listener = jnx_socket_tcp_listener_create("9991",AF_INET,15);
  
    /* Do nothing the session is fake */
  }
  return 0;
}
int unlinking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {

  return 0;
}
void test_joiner() {
  JNXLOG(NULL,"test_joiner");
  session_service *service = session_service_create(linking_test_procedure,
      unlinking_test_procedure);
  peerstore *store = peerstore_init(local_peer_for_user("initiator_bob",10,interface), 0);

  get_broadcast_ip(&baddr);
  printf("%s\n", baddr);
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

  /* create a fake session to invite the remote peer too */
  session *os;
  session_state e = session_service_create_session(service,&os);
 
  jnx_guid h;
  
  jnx_guid_create(&h);

  peer *fake_peer = peer_create(h,"111.112.113.114","FakeJoe",0);

  session_service_link_sessions(service,E_AM_INITIATOR,
      ds,&(*os).session_guid,local,remote_peer);

  /* here we have a fake session that does nothing, but lets tell remote peer 
   * about it */
  
  auth_comms_invite_send(ac,os,remote_peer);


  auth_comms_destroy(&ac);

}
int main(int argc, char **argv) {
  if (argc > 1) {
    interface = argc[1];
    puts("using interface %s", interface);
  }
  JNXLOG_CREATE("logger.conf");
  test_joiner();
  return 0;
}
