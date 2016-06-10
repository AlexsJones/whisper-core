#include "connection_request.h"
#include "encoding.h"
#include "peerstore.h"
connection_request *connection_request_create(peer *remote,
    const discovery_service *ds) {
  connection_request *r = malloc(sizeof(connection_request));
  peer *local = peerstore_get_local_peer(ds->peers);
  r->local = local;
  r->ds = ds;
  r->remote = remote;
  r->keypair = asymmetrical_generate_key(2048);
  jnx_guid_create(&(*r).id);
  jnx_char *connection_id;
  jnx_guid_to_string(&(*r).id,&connection_id);
  JNXLOG(LDEBUG, "Generated new connection with id %s", connection_id);
  JNXLOG(LDEBUG, "Connection local peer is %s",&(*r->local).guid);
  JNXLOG(LDEBUG, "Connection remote peer is %s",&(*r->remote).guid);
  free(connection_id);
  return r;
}
connection_request *connection_request_create_with_identity_chain(peer *remote,jnx_guid *id, const discovery_service *ds) {
  connection_request *c = connection_request_create(remote,ds);
  (*c).id = *id;
  return c;
}
void connection_request_update_state(connection_request *req, connection_request_state s) {
  req->state = s;
}
void connection_request_destroy(connection_request **req) {
  asymmetrical_destroy_key((*req)->keypair);
  JNXCHECK(*req);
  free(*req);
}
Wpmessage *connection_request_create_initiation_message(connection_request *req, 
    connection_request_state s) {
  Wpmessage *message = NULL;
  jnx_char *str1=NULL;
  jnx_char *str2=NULL;
  jnx_char *connection_id = NULL;
  JNXLOG(LDEBUG,"E_CRS_INITIAL_CHALLENGE");
  //Generate keypair and send over my public key
  jnx_char *public_key = asymmetrical_key_to_string(req->keypair,PUBLIC);
  jnx_size osize;
  jnx_uint8 *encoded_public_key = encode_from_string(public_key,
      strlen(public_key), &osize); 

  JNXLOG(LDEBUG,encoded_public_key);
  jnx_guid_to_string(&(*req->local).guid,&str1);
  jnx_guid_to_string(&(*req->remote).guid,&str2);

  JNXLOG(LDEBUG,"Local guid %s Remote guid %s",str1,str2);
  jnx_guid_to_string(&(*req).id,&connection_id);
  JNXLOG(LDEBUG, "Generating message with connection id %s", connection_id);
  JNXCHECK(connection_id);
  JNXCHECK(str1);
  JNXCHECK(str2);
  wp_generation_state w = wpprotocol_generate_message(&message,
      connection_id,
      str1,str2,
      encoded_public_key,osize + 1,
      SELECTED_ACTION__CREATE_SESSION);
  JNXLOG(LDEBUG,"Generated message E_CRS_INITIAL_CHALLENGE")
    JNXCHECK(w == E_WGS_OKAY);
  JNXLOG(LDEBUG,"Freeing public_key");
  free(public_key);
  JNXLOG(LDEBUG,"Freeing encoded public key");
  free(encoded_public_key);
  JNXLOG(LDEBUG,"Freeing str1");
  free(str1);
  JNXLOG(LDEBUG,"Freeing str2");
  free(str2);
  JNXLOG(LDEBUG,"Freeing connection_id");
  free(connection_id);
  JNXLOG(LDEBUG,"Returning newly created message");
  return message;
}

Wpmessage *connection_request_create_exchange_message(connection_request *req, Wpmessage *incoming_message, 
    connection_request_state s) {

  Wpmessage *message = NULL;
  jnx_char *str1=NULL;
  jnx_char *str2=NULL;
  jnx_char *connection_id = NULL;
  jnx_char *reply_public_key = NULL;

  switch(s) {

    case E_CRS_CHALLENGE_REPLY:
      JNXLOG(LDEBUG,"E_CRS_CHALLENGE_REPLY");
      connection_id = NULL;
      str1 = NULL;
      str2 = NULL;
      //Generate keypair and send over my public key
      reply_public_key = asymmetrical_key_to_string(req->keypair,PUBLIC);
      //Encrypt my key in the challengers public key
      // RSA *challenger_key_public = asymmetrical_key_from_string() 
      JNXLOG(LDEBUG,"Generated public key");
      jnx_guid_to_string(&(*req->local).guid,&str1);
      jnx_guid_to_string(&(*req->remote).guid,&str2);

      JNXLOG(LDEBUG,"Local guid %s Remote guid %s",str1,str2);
      jnx_guid_to_string(&(*req).id,&connection_id);
      JNXLOG(LDEBUG, "Generating message with connection id %s", connection_id);
      JNXCHECK(connection_id);
      JNXCHECK(str1);
      JNXCHECK(str2);
      wp_generation_state w = wpprotocol_generate_message(&message,
          connection_id,
          str2,str1, /* SWAP THE OUTGOING SENDER TO THE LOCAL PEER */
          reply_public_key,strlen(reply_public_key) +1,
          SELECTED_ACTION__RESPONDING_CREATED_SESSION);

      JNXCHECK(w == E_WGS_OKAY);
      break;
    case E_CRS_SESSION_KEY_SHARE:

      break;
  }
  if(reply_public_key) {
    free(reply_public_key);
  }
  if(str1) {
    free(str1);
  }
  if(str2) {
    free(str2);
  }
  if(connection_id) {
    free(connection_id);
  }
  JNXLOG(LDEBUG,"Returning newly created message");
  return message;
}
