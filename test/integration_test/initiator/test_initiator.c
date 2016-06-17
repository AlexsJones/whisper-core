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
#include <jnxc_headers/jnx_socket.h>
#include <jnxc_headers/jnx_thread.h>
#include <jnxc_headers/jnx_guid.h>
#include <jnxc_headers/jnx_log.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include "connection_controller.h"
#include "discovery.h"
static char *baddr = NULL;
static char *interface = NULL;
static running = 1;
connection_controller *connectionc;

void on_connection_incoming(const connection_request *c) {
  JNXLOG(LDEBUG,"Callback for incoming new connection");
}
void on_connection_completed(const connection_request *c) {
  JNXLOG(LDEBUG,"Callback for completed connection");
  
  connection_controller_remove_connection_request(connectionc,
      (connection_request*)c);
  JNXLOG(LDEBUG,"Connection complete!");
  running = 0;
}
void on_connection_closed(const connection_request *c) {
  JNXLOG(LDEBUG,"Callback for connection closed");
}
void test_initiator() {

  peerstore *store = peerstore_init(local_peer_for_user("initiator_bob",10,interface), 0);

  get_broadcast_ip(&baddr,interface);
  printf("%s\n", baddr);
  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  discovery_service_start(ds,BROADCAST_UPDATE_STRATEGY);

  // //CREATING WPPROTOCOL MUX

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

  connectionc = connection_controller_create("8080", 
      AF_INET, ds,
      on_connection_completed, on_connection_incoming,
      on_connection_closed);

  connection_request *request;
  connection_controller_initiation_request(connectionc,remote_peer, &request);

  while(running) {

    connection_controller_tick(connectionc);

    sleep(1);
  }

  connection_controller_destroy(&connectionc);
}
int main(int argc, char **argv) {


  if (argc > 1) {
    interface = argv[1];
    printf("using interface %s", interface);
  }
  test_initiator();
  return 0;
}
