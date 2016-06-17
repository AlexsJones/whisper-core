#include "connection_request.h"
#include "encoding.h"
#include "peerstore.h"
#include "encoding.h"
connection_request *connection_request_create(peer *remote,
    const discovery_service *ds) {
  connection_request *r = malloc(sizeof(connection_request));
  peer *local = peerstore_get_local_peer(ds->peers);
  r->local = local;
  r->ds = ds;
  r->remote = remote;
  r->shared_secret = NULL;
  r->keypair = asymmetrical_generate_key(2048);
  jnx_char *local_guid;
  jnx_char *remote_guid;
  jnx_guid_to_string(&(*r->local).guid,&local_guid); 
  jnx_guid_to_string(&(*r->remote).guid,&remote_guid); 
  JNXLOG(LDEBUG,"Created connection with local guid %s and remote guid %s",
      local_guid,remote_guid);
  free(local_guid);
  free(remote_guid);
  return r;
}
connection_request *connection_request_create_with_identity_chain(peer *remote,
    jnx_guid *id, const discovery_service *ds) {
  connection_request *c = connection_request_create(remote,ds);
  (*c).id = *id;
  return c;
}
void connection_request_update_state(connection_request *req, 
    connection_request_state s) {
  req->state = s;
}
void connection_request_destroy(connection_request **req) {
  asymmetrical_destroy_key((*req)->keypair);
  if((*req)->shared_secret) {
    free((*req)->shared_secret);
  }  
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
  JNXCHECK(req->keypair);
  jnx_char *public_key = asymmetrical_key_to_string(req->keypair,PUBLIC);
  jnx_size osize;
  jnx_uint8 *encoded_public_key = encode_from_string(public_key,
      strlen(public_key)+1, &osize); 

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
      encoded_public_key,osize,
      SELECTED_ACTION__CREATE_SESSION);
  JNXCHECK(w == E_WGS_OKAY);
  free(public_key);
  free(encoded_public_key);
  free(str1);
  free(str2);
  free(connection_id);
  return message;
}

Wpmessage *connection_request_create_exchange_message(connection_request *req, 
    Wpmessage *incoming_message, 
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
      jnx_size encoded_len;
      jnx_char *encoded = encode_from_string(reply_public_key,
          strlen(reply_public_key) +1,&encoded_len);
      jnx_guid_to_string(&(*req->local).guid,&str1);
      jnx_guid_to_string(&(*req->remote).guid,&str2);
      jnx_guid_to_string(&(*req).id,&connection_id);
      JNXCHECK(connection_id);
      JNXCHECK(str1);
      JNXCHECK(str2);
      wp_generation_state w = wpprotocol_generate_message(&message,
          connection_id,
          str1,str2,
          encoded,encoded_len,
          SELECTED_ACTION__RESPONDING_CREATED_SESSION);

      JNXCHECK(w == E_WGS_OKAY);
      free(encoded);
      break;
    case E_CRS_SESSION_KEY_SHARE:
      JNXLOG(LDEBUG,"E_CRS_SESSION_KEY_SHARE");
      connection_id = NULL;
      str1 = NULL;
      str2 = NULL;
      //Generate shared secret and encrypt it
      jnx_char *raw = incoming_message->action->contextdata->rawdata.data;
      jnx_size out_decoded;
      jnx_char *decoded = decode_to_string(raw,
          incoming_message->action->contextdata->rawdata.len,&out_decoded);
      JNXLOG(LDEBUG,"Decoded to string %s",decoded);
      JNXCHECK(decoded);
      RSA *remote_keypair = asymmetrical_key_from_string(decoded,PUBLIC);
      JNXLOG(LDEBUG,"Generated from remote public key");
      jnx_uint8 *buffer;
      jnx_char secret_size = generate_shared_secret(&buffer);
      JNXLOG(LDEBUG,"Generated shared secret %s",buffer);
      //encrypt shared secret & store in local session
      req->shared_secret = malloc(secret_size + 1);
      bzero(req->shared_secret,secret_size + 1);
      memcpy(req->shared_secret,buffer,secret_size);
      jnx_size asym_encrypted_size;
      JNXLOG(LDEBUG,"About to encrypt shared secret");
      JNXCHECK(remote_keypair);
      jnx_char *encrypted_string = asymmetrical_encrypt(remote_keypair,
          buffer,&asym_encrypted_size);
      jnx_size encoded_secret_len;
      JNXCHECK(encrypted_string);
      JNXLOG(LDEBUG,"Encrypted shared secret %s",encrypted_string);
      JNXLOG(LDEBUG,"About to encode encrypted shared secret");
      jnx_char *encoded_secret = encode_from_string(encrypted_string,
          asym_encrypted_size,&encoded_secret_len);
      JNXLOG(LDEBUG,"Encoded shared secret successfully");
      jnx_guid_to_string(&(*req->local).guid,&str1);
      jnx_guid_to_string(&(*req->remote).guid,&str2);
      jnx_guid_to_string(&(*req).id,&connection_id);
      JNXLOG(LDEBUG, "Generating message with connection id %s", connection_id);
      JNXCHECK(connection_id);
      JNXCHECK(str1);
      JNXCHECK(str2);
      w = wpprotocol_generate_message(&message,
          connection_id,
          str1,str2,
          encoded_secret,encoded_secret_len,
          SELECTED_ACTION__SHARING_SESSION_KEY);

      JNXCHECK(w == E_WGS_OKAY);
      free(encoded_secret);
      free(encrypted_string);
      free(decoded);
      free(buffer);
      break;

    case E_CRS_COMPLETE:
      JNXLOG(LDEBUG,"E_CRS_COMPLETE");
      jnx_guid_to_string(&(*req->local).guid,&str1);
      jnx_guid_to_string(&(*req->remote).guid,&str2);
      jnx_guid_to_string(&(*req).id,&connection_id);

      if(!req->shared_secret) {
        JNXLOG(LERROR,"Should not be sending a completion message with no shared key!");
        exit(1);
      }
      jnx_char *encrypted = symmetrical_encrypt(req->shared_secret,"OKAY",5);

      w = wpprotocol_generate_message(&message,
          connection_id,str1,str2,
          encrypted,strlen(encrypted),SELECTED_ACTION__COMPLETED_SESSION);
      break;
  }
  JNXLOG(LDEBUG,"=====Created new exchange message=====");
  JNXLOG(LDEBUG,"Sender: %s",str1);
  JNXLOG(LDEBUG,"Recipient: %s",str2);
  JNXLOG(LDEBUG,"======================================");
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
  return message;
}
