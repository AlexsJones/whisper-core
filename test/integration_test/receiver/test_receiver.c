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
#include <jnxc_headers/jnx_log.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include "discovery.h"
#include "connection_controller.h"

static char *baddr = NULL;
static char *interface = NULL;

void on_connection_incoming(const connection_request *c) {
  JNXLOG(LDEBUG,"Callback for incoming new connection");
}
void on_connection_completed(const connection_request *c) {
  JNXLOG(LDEBUG,"Callback for completed connection");
}
void on_connection_closed(const connection_request *c) {
  JNXLOG(LDEBUG,"Callback for connection closed");
}
void test_receiver() {
  JNXLOG(NULL, "test_linking");
 
  jnx_guid h;
  jnx_guid_create(&h);

  peerstore *store = peerstore_init(local_peer_for_user("receiver_bob", 10,interface), 0);

  get_broadcast_ip(&baddr,interface);

  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  discovery_service_start(ds, BROADCAST_UPDATE_STRATEGY);

  connection_controller *connectionc = connection_controller_create("8080", AF_INET, ds,
      on_connection_completed, on_connection_incoming, on_connection_closed);


  while (1) {

    connection_controller_tick(connectionc);
 
  }
  connection_controller_destroy(&connectionc);
}

int main(int argc, char **argv) {
  if (argc > 1) {
    interface = argv[1];
    printf("using interface %s", interface);
  }
  test_receiver();
  return 0;
}
