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
#include <whisper_protocol_headers/wpmux.h>
#include <whisper_protocol_headers/wpprotocol.h>
#include "session_service.h"
#include "port_control.h"
#include "discovery.h"
static char *baddr = NULL;
static char *interface = NULL;

static wp_mux *mux;

void send_message(Wpmessage *message, void *optargs) {

  JNXLOG(LDEBUG,"Emitting message!");
 discovery_service *ds = (discovery_service*)optargs;
 jnx_size osize;
 jnx_char *obuffer;
 wp_generation_state e = wpprotocol_generate_message_string(message,
     &obuffer,&osize); 

  JNXLOG(LDEBUG,"Generated message string");
  //get recipient guid..
  jnx_guid rguid;
  jnx_guid_from_string(message->recipient,&rguid); 

  JNXLOG(LDEBUG,"Remote guid is %s", message->recipient);

  peer *remote_peer = peerstore_lookup(ds->peers,&rguid);

  jnx_socket *sock = jnx_socket_tcp_create(AF_INET);
  jnx_socket_tcp_send(sock,remote_peer->host_address,"8080",obuffer,osize);
  jnx_socket_destroy(&sock);

  JNXLOG(LDEBUG,"Sent message of size %zu",osize);
}
int linking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {
  if(session_type == E_AM_INITIATOR){
    JNXCHECK(session_type == E_AM_INITIATOR);
    JNXLOG(NULL,"Session hit linking procedure functor");

    discovery_service *ds = (discovery_service*)optargs;
    jnx_size *msg_size;

    jnx_char *message;
    jnx_char *data = malloc(strlen("Hello"));
    bzero(data,6);
    memcpy(data,"Hello",6);

    peer *local_peer = peerstore_get_local_peer(ds->peers);
    peer *remote_peer = peerstore_lookup(ds->peers,&(*s).remote_peer_guid);

   jnx_char *str;
   jnx_guid_to_string(&(*local_peer).guid,&str);
   jnx_char *str2;
   jnx_guid_to_string(&(*remote_peer).guid,&str2);


    wp_generation_state w = wpprotocol_generate_message(&message,str,str2,
        data,6,SELECTED_ACTION__CREATE_SESSION);

    JNXCHECK(w == E_WGS_OKAY);

    wpprotocol_mux_push(mux,message);


    Wpmessage *omessage;

    while(wpprotocol_mux_pop(mux,&omessage) == E_WMS_OKAY_EMPTY)  {
    
      wpprotocol_mux_tick(mux);
      sleep(1000);
    }
    
    JNXLOG(LDEBUG,"Received reply!");
    
    /* Adding port control service */
    /*
       int init_port = rand() % 1000;

       port_control_service *ps = port_control_service_create(8000 + 
       init_port,
       12341,1);

       ac = auth_comms_create();

       ac->listener = jnx_socket_tcp_listener_create("9991",AF_INET,15);
       auth_comms_initiator_start(ac,ds,ps,s,"Hello from the initiator!");
       */
  }
  return 0;
}

int unlinking_test_procedure(session *s,linked_session_type session_type,
    void *optargs) {

//  auth_comms_stop(ac,s);

  return 0;
}
void test_initiator() {
  JNXLOG(NULL,"test_linking");
  session_service *service = session_service_create(linking_test_procedure,
      unlinking_test_procedure);
  session *os;
  session_state e = session_service_create_session(service,&os);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 0);
  //Lets generate the guid of some remote session
  jnx_guid h;
  jnx_guid_create(&h);

  peerstore *store = peerstore_init(local_peer_for_user("initiator_bob",10,interface), 0);

  get_broadcast_ip(&baddr,interface);
  printf("%s\n", baddr);
  discovery_service *ds = discovery_service_create(1234, AF_INET, baddr, store);

  discovery_service_start(ds,BROADCAST_UPDATE_STRATEGY);

  //CREATING WPPROTOCOL MUX
  mux = wpprotocol_mux_create("8080",AF_INET,send_message,ds);
  
  
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
  JNXCHECK(session_is_active(os) == 0);

  session_service_link_sessions(service,E_AM_INITIATOR,
      ds,&(*os).session_guid,local,remote_peer);

  JNXCHECK(session_is_active(os) == 1);

  printf("-------------------------------------\n");
  session_message_write(os,"Hello Ballface! what's going on!");
  printf("-------------------------------------\n");
  printf("Sessions linked - now going to unlink\n");

  session_service_unlink_sessions(service,E_AM_INITIATOR,
      ds,&(*os).session_guid);

  JNXCHECK(session_is_active(os) == 0);

  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 0);
}
int main(int argc, char **argv) {
  

  if (argc > 1) {
    interface = argv[1];
    printf("using interface %s", interface);
  }
  test_initiator();
  wpprotocol_mux_destroy(&mux);
  return 0;
}
