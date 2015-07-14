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
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include "session_service.h"
#include "auth_comms.h"
#include "discovery.h"

static char *baddr = NULL;
static char *interface = NULL;
static auth_comms_service *ac;
int accept_invite_callback(jnx_guid *session_guid) {

  return 1;
}
int linking_test_procedure(session *s, linked_session_type session_type,
    void *optargs) {
  JNXLOG(NULL, "Linking now the receiver session..");
  /*
     jnx_char *default_secure_comms = "6666";
     auth_comms_service *ac = auth_comms_create();
     ac->listener = jnx_socket_tcp_listener_create("9991",AF_INET,15);
     auth_comms_initiator_start(ac,ds,os,default_secure_comms);
     */
  return 0;
}

int unlinking_test_procedure(session *s, linked_session_type session_type,
    void *optargs) {

  auth_comms_stop(ac,s);
  return 0;
}

int app_accept_reject(discovery_service *ds, jnx_guid *initiator_guild,
    jnx_guid *session_guid) {
  return 0;
}

void test_receiver() {
  JNXLOG(NULL, "test_linking");
  session_service *service = session_service_create(linking_test_procedure,
      unlinking_test_procedure);

  //Lets generate the guid of some remote session
  jnx_guid h;
  jnx_guid_create(&h);

  peerstore *store = peerstore_init(local_peer_for_user("receiver_bob", 10,interface), 0);

  get_broadcast_ip(&baddr,interface);

  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  discovery_service_start(ds, BROADCAST_UPDATE_STRATEGY);

  ac = auth_comms_create();
  ac->ar_callback = app_accept_reject;
  ac->invitation_callback = accept_invite_callback;
  ac->listener = jnx_socket_tcp_listener_create("9991", AF_INET, 15);
  auth_comms_listener_start(ac, ds, service, NULL);


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


  jnx_guid_create(&h);

  peer *fake_peer = peer_create(h,"111.112.113.114","FakeJoe",0);

  session_service_link_sessions(service,E_AM_INITIATOR,
      ds,&(*os).session_guid,local,remote_peer);

  /* here we have a fake session that does nothing, but lets tell remote peer 
   * about it */

  auth_comms_invite_send(ac,os,remote_peer);

start:
  while (1) {

    jnx_list *olist = NULL;

    if (session_service_fetch_all_sessions(service,
          &olist) != SESSION_STATE_NOT_FOUND) {

      printf("-----------------------------------------------\n");
      session *s = jnx_list_remove_front(&olist);

      while (!session_is_active(s)){
        sleep(1);
      }

      jnx_char *buffy = NULL;

      int size = 0;
      while (size <= 0) {
        size = session_message_read(s, &buffy);
      }
      if(size) {
        printf("size -> %d, buffy -> %s\n", size, buffy);

        JNXCHECK(session_is_active(s) == 1);

        session_service_unlink_sessions(service,E_AM_RECEIVER,
            ds,&(*s).session_guid);

        JNXCHECK(session_is_active(s) == 0);

        session_service_destroy_session(service,&(*s).session_guid);

        jnx_list_destroy(&olist);

        break;
      }    
    }
    if(olist)
      jnx_list_destroy(&olist);
  }

}

int main(int argc, char **argv) {
  if (argc > 1) {
    interface = argv[1];
    printf("using interface %s", interface);
  }
  JNXLOG_CREATE("logger.conf");
  test_receiver();
  return 0;
}
