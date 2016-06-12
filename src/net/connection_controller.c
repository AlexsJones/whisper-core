#include "connection_controller.h"
#include "encoding.h"
#include <jnxc_headers/jnx_check.h>
#include <jnxc_headers/jnx_guid.h>
void internal_connection_control_emitter(Wpmessage *message, void *opt_args){
  connection_controller *controller = (connection_controller*)opt_args;
  jnx_size osize;
  jnx_char *obuffer;
  wp_generation_state e = wpprotocol_generate_message_string(message,
      &obuffer,&osize); 
  jnx_guid rguid;
  jnx_guid_from_string(message->recipient,&rguid); 
  peer *remote_peer = peerstore_lookup(controller->ds->peers,&rguid);
  jnx_socket *sock = jnx_socket_tcp_create(AF_INET);
  JNXLOG(LDEBUG,"Sending message to %s", remote_peer->host_address);
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
  //Get the remote sender as a peer
  jnx_guid message_guid;
  jnx_guid_from_string(message->sender,&message_guid);
  peer *remote = peerstore_lookup(controller->ds->peers,&message_guid);
  peer *local = peerstore_get_local_peer(controller->ds->peers);
  JNXCHECK(remote);
  if (jnx_guid_compare(&(*remote).guid,&(*local).guid) == JNX_GUID_STATE_SUCCESS) {
    JNXFAIL("A message has been sent from this node to this node");
  }
  //Fetch the existing connection if it exists
  jnx_node *h = controller->connections->head;
  jnx_node *r = controller->connections->head;
  connection_request *oconnection = NULL;  
  jnx_guid incoming_connection_guid;
  jnx_guid_from_string(message->id,&incoming_connection_guid);
  while(h != NULL) {   
    connection_request *c = (connection_request*)h->_data;
    if(jnx_guid_compare(&(*c).id,&incoming_connection_guid) == JNX_GUID_STATE_SUCCESS) {
      JNXLOG(LDEBUG,"Found existing connection %s",message->id);    
      oconnection = c;      
      break;
    }
    h = h->next_node;
  }
  h = r;

  JNXLOG(LDEBUG,"Received message with connection id %s",message->id);
  JNXLOG(LDEBUG,"Message sender is %s",message->sender);
  JNXLOG(LDEBUG,"Message sender host_address is %s", remote->host_address);
  JNXLOG(LDEBUG, "Message raw payload [%s]",message->action->contextdata->rawdata.data);

  Wpmessage *out_message = NULL;
  switch(message->action->action) {
    case SELECTED_ACTION__CREATE_SESSION:
      if(oconnection) {
        JNXLOG(LERROR,"There should not be an existing connection");
        exit(1);
      }
      JNXLOG(LDEBUG,"Message action -> SELECTED_ACTION__CREATE_SESSION");
      jnx_guid message_id_guid;
      jnx_guid_from_string(message->id,&message_id_guid);
      connection_request *c = connection_request_create_with_identity_chain(
          remote,&message_id_guid,controller->ds);      
      //Update connection state-------
      c->state = E_CRS_CHALLENGE_REPLY;
      //------------------------------
      out_message = connection_request_create_exchange_message(c,message,E_CRS_CHALLENGE_REPLY);
      JNXCHECK(out_message);
      wpprotocol_mux_push(controller->mux,out_message);
      JNXCHECK(connection_controller_add_connection_request(controller,c) == E_CCS_OKAY);
      break;

    case SELECTED_ACTION__RESPONDING_CREATED_SESSION:
      if(!oconnection) {

        JNXLOG(LERROR,"Found an reply without an existing connection");
        exit(1);
      }
      //Update connection state-------
      oconnection->state = E_CRS_SESSION_KEY_SHARE;
      //------------------------------
      JNXLOG(LDEBUG,"Message action -> SELECTED_ACTION__RESPONDING_CREATED_SESSION");
      out_message = connection_request_create_exchange_message(oconnection,message,
          E_CRS_SESSION_KEY_SHARE);
      wpprotocol_mux_push(controller->mux,out_message);
      break;

    case SELECTED_ACTION__SHARING_SESSION_KEY:
      JNXCHECK(oconnection);
      JNXLOG(LDEBUG,"Message action -> SELECTED_ACTION__SHARING_SESSION_KEY");
      //Update connection state-------
      oconnection->state = E_CRS_COMPLETE;
      //------------------------------
      jnx_size decoded_len;
      jnx_char *decoded_key = decode_to_string(message->action->contextdata->rawdata.data,
        message->action->contextdata->rawdata.len,&decoded_len);
      jnx_size decrypted_key_len;
      jnx_char *decrypted_key = asymmetrical_decrypt(oconnection->keypair,decoded_key,
        decoded_len,&decrypted_key_len);


      free(decoded_key);
      free(decrypted_key);
      JNXLOG(LDEBUG,"Completed with shared session key of %s",decrypted_key);
      break;
  }

  wpmessage__free_unpacked(message,NULL);
}

connection_controller *connection_controller_create(jnx_char *traffic_port, 
    jnx_uint8 family, 
    const discovery_service *ds) {
  connection_controller *controller = malloc(sizeof(connection_controller));
  controller->ds = ds;
  controller->port = strdup(traffic_port);
  controller->connections = jnx_list_create();
  controller->mux = wpprotocol_mux_create(traffic_port, 
      family,internal_connection_control_emitter, controller);
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

connection_controller_state connection_controller_initiation_request(
    connection_controller *controller, peer *remote, connection_request **outrequest) {
  connection_request *c = connection_request_create(remote,controller->ds);
  //Update connection state
  c->state = E_CRS_INITIAL_CHALLENGE;
  jnx_char *connection_id;
  jnx_guid_to_string(&(*c).id,&connection_id);
  JNXLOG(LDEBUG, "Generated new connection with id %s", connection_id);
  free(connection_id);

  Wpmessage *message = connection_request_create_initiation_message(c,E_CRS_INITIAL_CHALLENGE);
  JNXCHECK(message);
  JNXLOG(LDEBUG,"Pushing new message into mux");
  wpprotocol_mux_push(controller->mux,message);

  JNXCHECK(connection_controller_add_connection_request(controller,c) == E_CCS_OKAY);

  return E_CCS_OKAY;
}
connection_request_state connection_controller_fetch_state(
    connection_request *request) {
  return request->state;
}
connection_controller_state connection_controller_add_connection_request(
    connection_controller *controller,
    connection_request *c) {
  jnx_list_add(controller->connections,c);
  return E_CCS_OKAY;
}
connection_controller_state connection_controller_remove_connection_request(
    connection_controller *controller,
    connection_request *c) {

  return E_CCS_OKAY;
}
