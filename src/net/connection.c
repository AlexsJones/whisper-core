#include "connection.h"

void internal_connection_control_emitter(Wpmessage *message, void *opt_args){

  connection_controller *controller = (connection_controller*)opt_args;
  jnx_size osize;
  jnx_char *obuffer;
  wp_generation_state e = wpprotocol_generate_message_string(message,
      &obuffer,&osize); 

  JNXLOG(LDEBUG,"Generated message string");
  //get recipient guid..
  jnx_guid rguid;
  jnx_guid_from_string(message->recipient,&rguid); 

  JNXLOG(LDEBUG,"Remote guid is %s", message->recipient);

  peer *remote_peer = peerstore_lookup(controller->ds->peers,&rguid);

  jnx_socket *sock = jnx_socket_tcp_create(AF_INET);
  jnx_socket_tcp_send(sock,remote_peer->host_address,controller->port,obuffer,osize);
  jnx_socket_destroy(&sock);

  free(obuffer);
  wpmessage__free_unpacked(message,NULL);
  JNXLOG(LDEBUG,"Sent message of size %zu",osize);
}

void internal_connnection_message_processor(connection_controller *controller, Wpmessage *message){



}

connection_controller *connection_controller_create(jnx_char *traffic_port, jnx_uint8 family, 
    const discovery_service *ds) {
  connection_controller *controller = malloc(sizeof(connection_controller));
  controller->ds = ds;
  controller->port = strdup(traffic_port);
  controller->mux = wpprotocol_mux_create(traffic_port, family,internal_connection_control_emitter, controller);
  return controller;
}

void *connection_controller_destroy(connection_controller **controller) {

  wpprotocol_mux_destroy(&(*controller)->mux);
  free(*controller);
  *controller = NULL; 
}

void connection_controller_tick(connection_controller *controller) {

  wpprotocol_mux_tick(controller->mux);

  Wpmessage *outmessage;
  wp_mux_state state = wpprotocol_mux_pop(controller->mux,&outmessage);
  if(state == E_WMS_OKAY) {
  	if(!outmessage) {
  		JNXLOG(LWARN, "Message was expected for processing - was null");
    	return;
    }
    internal_connnection_message_processor(controller, outmessage);
  }
}

const connection_request* connection_controller_initiation_request(connection_controller *controller, 
    peer *local, peer *remote) {


}

connection_request_state connection_controller_fetch_state(connection_request *request) {

  return request->state;
}
