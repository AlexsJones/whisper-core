#include "connection_controller.h"
#include <jnxc_headers/jnx_check.h>
#include <jnxc_headers/jnx_guid.h>
void internal_connection_control_emitter(Wpmessage *message, void *opt_args){

  connection_controller *controller = (connection_controller*)opt_args;
  jnx_size osize;
  jnx_char *obuffer;
  wp_generation_state e = wpprotocol_generate_message_string(message,
      &obuffer,&osize); 
  JNXLOG(LDEBUG,"Generated message string");
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

void internal_connnection_message_processor(connection_controller *controller,
    Wpmessage *message){
  JNXCHECK(controller);
  if(!message) {
    JNXLOG(LERROR,
        "Malformed message receieved by internal_connnection_message_processor");
    return;
  }

  //Fetch the existing connection if it exists
  jnx_node *h = controller->connections->head;
  jnx_node *r = controller->connections->head;
  connection_request *oconnection = NULL;

  jnx_guid message_guid;
  jnx_guid_from_string(message->id,&message_guid);
  while(h != NULL) {

    connection_request *c = (connection_request*)h->_data;
    if(jnx_guid_compare(&(*c).id,&message_guid) == JNX_GUID_STATE_SUCCESS) {
      JNXLOG(LDEBUG,"Found existing connection!");    
      oconnection = c;      
      break;
    }
    h = h->next_node;
  }
  h = r;

  switch(message->action->action) {
    case SELECTED_ACTION__CREATE_SESSION:
      //Create a new connection and add it to our connection pool
      JNXLOG(LDEBUG,"Message action -> SELECTED_ACTION__CREATE_SESSION");
      break;
    case SELECTED_ACTION__RESPONDING_CREATED_SESSION:
      JNXLOG(LDEBUG,"Message action -> SELECTED_ACTION__RESPONDING_CREATED_SESSION");
      //Update connection status with 
      break;
    case SELECTED_ACTION__SHARING_SESSION_KEY:
      JNXCHECK(oconnection);
      JNXLOG(LDEBUG,"Message action -> SELECTED_ACTION__SHARING_SESSION_KEY");

      break;
      JNXCHECK(oconnection);
    case SELECTED_ACTION__COMPLETED_SESSION:
      JNXLOG(LDEBUG,"Message action -> SELECTED_ACTION__COMPLETED_SESSION");

      break;
  }

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
  JNXCHECK(*controller);
  jnx_list_destroy(&(*controller)->connections);
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

connection_controller_state connection_controller_initiation_request(connection_controller *controller, 
    peer *local, peer *remote, connection_request **outrequest) {
  
  connection_request *c = connection_request_create();
  // //Message
  jnx_char *data = malloc(strlen("Hello"));
  bzero(data,6);
  memcpy(data,"Hello",6);
  //Action state
  jnx_char *str1;
  jnx_guid_to_string(&(*local).guid,&str1);
  jnx_char *str2;
  jnx_guid_to_string(&(*remote).guid,&str2);
  Wpmessage *message;


  jnx_char *connection_id;
  jnx_guid_to_string(&(*c).id,&connection_id);
  wp_generation_state w = wpprotocol_generate_message(&message,
    connection_id,
    str1,str2,
      data,6,SELECTED_ACTION__CREATE_SESSION);
  free(data);
  free(str1);
  free(str2);
  if(!message) {

    connection_request_destroy(&c);
    return E_CCS_FAILED;
  }
  
  wpprotocol_mux_push(controller->mux,message);
  JNXCHECK(connection_controller_add_connection_request(controller,c) == E_CCS_OKAY);
  connection_request_update_state(c, E_CRS_PREHANDSHAKE);
  return E_CCS_OKAY;
}
connection_request_state connection_controller_fetch_state(connection_request *request) {

  return request->state;
}
connection_controller_state connection_controller_add_connection_request(connection_controller *controller,
  connection_request *c) {
  jnx_list_add(controller->connections,c);
  return E_CCS_OKAY;
}
connection_controller_state connection_controller_remove_connection_request(connection_controller *controller,
  connection_request *c) {

  return E_CCS_OKAY;
}