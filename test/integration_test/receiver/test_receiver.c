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
#include "session_controller.h"

static char *baddr = NULL;
static char *interface = NULL;
static connection_controller *connectionc;
static session_controller *sc;
static int complete = 0;
void on_message_input(const session *s, const connection_request *r, jnx_char *message, jnx_size len) {

  complete = 1;
  printf("on_session_message: %s\n",message);
  JNXLOG(LDEBUG,"Successfully receieved message through session!");
    session_controller_destroy(&sc);

    connection_controller_destroy(&connectionc);
  
}

void test_receiver() {
  JNXLOG(NULL, "test_linking");

  jnx_guid h;
  jnx_guid_create(&h);

  peerstore *store = peerstore_init(local_peer_for_user("receiver_bob", 10,interface), 0);

  get_broadcast_ip(&baddr,interface);

  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  discovery_service_start(ds, BROADCAST_UPDATE_STRATEGY);

  connectionc = connection_controller_create("8080", AF_INET, ds,
      NULL,NULL,NULL,NULL);

  sc = session_controller_create(connectionc,on_message_input);


  while (!complete) {

    if(connectionc)
      connection_controller_tick(connectionc);
    sleep(1);
  }

}

int main(int argc, char **argv) {
  if (argc > 1) {
    interface = argv[1];
    printf("using interface %s", interface);
  }
  test_receiver();
  return 0;
}
