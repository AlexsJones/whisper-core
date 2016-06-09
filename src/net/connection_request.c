#include "connection_request.h"

connection_request *connection_request_create(peer *local, peer *remote,const discovery_service *ds) {
  connection_request *r = malloc(sizeof(connection_request));
  r->local = local;
  r->ds = ds;
  r->remote = remote;
  r->keypair = asymmetrical_generate_key(2048);
  jnx_guid_create(&(*r).id);
  jnx_char *connection_id;
  jnx_guid_to_string(&(*r).id,&connection_id);
  JNXLOG(LDEBUG, "Generated new connection with id %s", connection_id);
  free(connection_id);
  return r;
}
void connection_request_update_state(connection_request *req, connection_request_state s) {
  req->state = s;
}
void connection_request_destroy(connection_request **req) {
  asymmetrical_destroy_key((*req)->keypair);
  JNXCHECK(*req);
  free(*req);
}
Wpmessage *connection_request_create_message(connection_request *req, connection_request_state s) {

  Wpmessage *message = NULL;

  switch(s) {
    case E_CRS_INITIAL_CHALLENGE:
      JNXLOG(LDEBUG,"E_CRS_INITIAL_CHALLENGE");
      //Generate keypair and send over my public key
      jnx_char *public_key = asymmetrical_key_to_string(req->keypair,PUBLIC);
      
      jnx_char *str1;
      jnx_guid_to_string(&(*req->local).guid,&str1);
      jnx_char *str2;
      jnx_guid_to_string(&(*req->remote).guid,&str2);

      JNXLOG(LDEBUG,"Local guid %s Remote guid %s",str1,str2);
      jnx_char *connection_id;
      jnx_guid_to_string(&(*req).id,&connection_id);
      JNXLOG(LDEBUG, "Generating message with connection id %s", connection_id);
      JNXCHECK(connection_id);
      JNXCHECK(str1);
      JNXCHECK(str2);
      wp_generation_state w = wpprotocol_generate_message(&message,
          connection_id,
          str1,str2,
          public_key,strlen(public_key),
          SELECTED_ACTION__CREATE_SESSION);
      JNXCHECK(w == E_WGS_OKAY);
      free(public_key);
      free(str1);
      free(str2);
      free(connection_id);
      break;

    case E_CRS_CHALLENGE_REPLY:
       JNXLOG(LDEBUG,"E_CRS_CHALLENGE_REPLY");
      connection_id = NULL;
      str1 = NULL;
      str2 = NULL;
      //Generate keypair and send over my public key
      jnx_char *reply_public_key = asymmetrical_key_to_string(req->keypair,PUBLIC);
      //Encrypt my key in the challengers public key
     // RSA *challenger_key_public = asymmetrical_key_from_string() 


      jnx_guid_to_string(&(*req->local).guid,&str1);
      jnx_guid_to_string(&(*req->remote).guid,&str2);

      JNXLOG(LDEBUG,"Local guid %s Remote guid %s",str1,str2);
      jnx_guid_to_string(&(*req).id,&connection_id);
      JNXLOG(LDEBUG, "Generating message with connection id %s", connection_id);
      JNXCHECK(connection_id);
      JNXCHECK(str1);
      JNXCHECK(str2);
      w = wpprotocol_generate_message(&message,
          connection_id,
          str1,str2,
          reply_public_key,strlen(reply_public_key),
          SELECTED_ACTION__RESPONDING_CREATED_SESSION);
      JNXCHECK(w == E_WGS_OKAY);
      free(public_key);
      free(str1);
      free(str2);
      free(connection_id);
      break;
    case E_CRS_SESSION_KEY_SHARE:

      break;
  }
  return message;
}
