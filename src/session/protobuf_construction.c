/*
 * session_service_auth_comms.c
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */
#include <jnxc_headers/jnxlog.h>
#include "protobuf_construction.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "cryptography.h"
#include "utils.h"
#include "auth_initiator.pb-c.h"
#include "auth_receiver.pb-c.h"
#include "auth_joiner.pb-c.h"
#include "auth_invite.pb-c.h"
#include "secure_comms_object.pb-c.h"


int protobuf_construction_did_receive_initiator_request(jnx_uint8 *obuffer,
    jnx_size bytes_read,
    void **oobject){
  *oobject = NULL;
  AuthInitiator *a = auth_initiator__unpack(NULL,bytes_read,obuffer);
  if(a == NULL) {
    return 0;
  }
  *oobject = a;
  return 1;
}
int protobuf_construction_did_receive_receiver_request(jnx_uint8 *obuffer,
    jnx_size bytes_read,void **oobject) {
  *oobject = NULL;
  AuthReceiver *a = auth_receiver__unpack(NULL,bytes_read,obuffer);
  if(a == NULL) {
    return 0;
  }
  *oobject = a;
  return 1;
}
int protobuf_construction_did_receive_invite_request(jnx_uint8 *obuffer,
    jnx_size bytes_read, void **oobject) {
  *oobject = NULL;
  AuthInvite *a = auth_invite__unpack(NULL,bytes_read,obuffer);
  if(a == NULL) {
    return 0;
  }
  *oobject = a;
  return 1;
}
int protobuf_construction_did_receive_joiner_request(jnx_uint8 *obuffer,
    jnx_size bytes_read,void **oobject) {
  *oobject = NULL;
  AuthJoiner *a = auth_joiner__unpack(NULL,bytes_read,obuffer);
  if(a == NULL) {
    return 0;
  }
  *oobject = a;
  return 1;
}
int protobuf_construction_did_receive_secure_comms_object(jnx_uint8 *obuffer,
    jnx_size bytes_read, void **oobject) {
  *oobject = NULL;
  SecureCommsObject *sco = secure_comms_object__unpack(NULL,bytes_read,obuffer);
  if(sco == NULL) {
    return 0;
  }
  *oobject = sco;
  return 1;
}
protobuf_construction_request_type protobuf_construction_resolve_request_type(jnx_uint8 *obuffer,
    jnx_size bytes_read, void **object) {
  *object = NULL;
  if(protobuf_construction_did_receive_initiator_request(obuffer,
        bytes_read,object)) {
    return REQUEST_TYPE_INITIATOR;
  }
  if(protobuf_construction_did_receive_receiver_request(obuffer,
        bytes_read,object)) {
    return REQUEST_TYPE_RECEIVER;
  }
  if(protobuf_construction_did_receive_invite_request(obuffer,
        bytes_read,object)) {
    return REQUEST_TYPE_INVITE;
  }
  if(protobuf_construction_did_receive_joiner_request(obuffer,
        bytes_read,object)) {
    return REQUEST_TYPE_JOINER;
  } 
  return REQUEST_TYPE_UNKNOWN;
}
int protobuf_construction_initiator_command_generate(session *ses,\
    protobuf_construction_initiator_state state,\
    jnx_uint8 *shared_secret,jnx_size secret_len,
    jnx_uint8 *initiator_message,
    jnx_uint8 **onetbuffer) {

  jnx_char *local_guid_str;
  jnx_guid_to_string(&(*ses).local_peer_guid,&local_guid_str);
  jnx_size len = strlen(local_guid_str);

  AuthInitiator auth_parcel = AUTH_INITIATOR__INIT;
  auth_parcel.initiator_guid = malloc(sizeof(char) * len + 1);
  memcpy(auth_parcel.initiator_guid,local_guid_str,len + 1); 
  free(local_guid_str);
  switch(state) {
    case CHALLENGE_PUBLIC_KEY:
      JNXLOG(LDEBUG,"Generating initial challenge flags.\n");
      auth_parcel.is_requesting_public_key = 1;
      auth_parcel.is_requesting_finish = 0;
      if(initiator_message) {
        auth_parcel.initiator_message = malloc(strlen(initiator_message) + 1);
        memcpy(auth_parcel.initiator_message,initiator_message,
            strlen(initiator_message)+1);
      }else {
        JNXLOG(LINFO,"protobuf_construction_initiator_command_generate: packed without initiator message");
      }
      break;
    case CHALLENGE_FINISH:
      JNXLOG(LDEBUG,"Generating finish request flags.\n");
      auth_parcel.is_requesting_public_key = 0;
      auth_parcel.is_requesting_finish = 1;

      auth_parcel.shared_secret.len = secret_len;
      auth_parcel.shared_secret.data = malloc(sizeof(char) * auth_parcel.shared_secret.len);

      JNXLOG(LDEBUG,"Transmitting encrypted shared secret with size %zu\n",auth_parcel.shared_secret.len);

      jnx_int i;
      for(i=0;i<auth_parcel.shared_secret.len;++i) {
          auth_parcel.shared_secret.data[i] = shared_secret[i];
      }

      auth_parcel.has_shared_secret = 1;

      JNXLOG(LDEBUG,"Setting shared secret len in proto to %zu\n",
          auth_parcel.shared_secret.len);
      break;
  }

  /* public key */
  jnx_char *pub_key_str = asymmetrical_key_to_string(ses->keypair,PUBLIC);
  jnx_size pub_len = strlen(pub_key_str);
  auth_parcel.initiator_public_key = malloc(sizeof(char*) * pub_len);
  memcpy(auth_parcel.initiator_public_key,pub_key_str,pub_len);
  free(pub_key_str);

  /*session guid */
  jnx_char *session_guid_str;
  jnx_size session_guid_len;
  jnx_guid_to_string(&ses->session_guid,&session_guid_str);
  session_guid_len = strlen(session_guid_str);
  auth_parcel.session_guid = malloc(sizeof(char*) * session_guid_len + 1);
  memcpy(auth_parcel.session_guid,session_guid_str,session_guid_len + 1);
  free(session_guid_str);

  /* secure comms port */
  jnx_size secure_comms_port_len = strlen(ses->secure_comms_port);
  auth_parcel.secure_comms_port = malloc(sizeof(char*) * secure_comms_port_len + 1);
  bzero(auth_parcel.secure_comms_port,secure_comms_port_len +1);
  memcpy(auth_parcel.secure_comms_port,ses->secure_comms_port,secure_comms_port_len + 1);

  /* packing */
  jnx_int parcel_len = auth_initiator__get_packed_size(&auth_parcel);
  jnx_uint8 *obuffer = malloc(parcel_len);
  auth_initiator__pack(&auth_parcel,obuffer);

  free(auth_parcel.initiator_guid);
  free(auth_parcel.initiator_public_key);

  *onetbuffer = obuffer;
  return parcel_len;
}
int protobuf_construction_receiver_command_generate(session *ses, \
    protobuf_construction_receiver_state state, jnx_int abort,jnx_uint8 **onetbuffer) {

  jnx_char *local_guid_str;
  jnx_guid_to_string(&(*ses).local_peer_guid,&local_guid_str);
  jnx_size len = strlen(local_guid_str);

  AuthReceiver auth_parcel = AUTH_RECEIVER__INIT;
  auth_parcel.receiver_guid = malloc(sizeof(char) * len);
  memcpy(auth_parcel.receiver_guid,local_guid_str,len); 
  free(local_guid_str);
  switch(state) {
    case RESPONSE_PUBLIC_KEY:
      JNXLOG(LDEBUG,"Generating public key response\n");
      auth_parcel.is_receiving_public_key = 1;
      auth_parcel.is_receiving_finish = 0;
      break;
    case RESPONSE_FINISH:
      JNXLOG(LDEBUG,"Generating finish response\n");
      auth_parcel.is_receiving_public_key = 0;
      auth_parcel.is_receiving_finish = 1;
      break;
  }
  /* set the abort token */
  auth_parcel.should_abort = abort;
  /* public key */
  jnx_char *pub_key_str = asymmetrical_key_to_string(ses->keypair,PUBLIC);
  jnx_size pub_len = strlen(pub_key_str);
  auth_parcel.receiver_public_key = malloc(sizeof(char*) * pub_len);
  memcpy(auth_parcel.receiver_public_key,pub_key_str,pub_len);
  free(pub_key_str);

  /*session guid */
  jnx_char *session_guid_str;
  jnx_size session_guid_len;
  jnx_guid_to_string(&ses->session_guid,&session_guid_str);
  session_guid_len = strlen(session_guid_str);

  auth_parcel.session_guid = malloc(sizeof(char*) * session_guid_len + 1);
  memcpy(auth_parcel.session_guid,session_guid_str,session_guid_len +1);
  free(session_guid_str);
  /* packing */
  jnx_int parcel_len = auth_receiver__get_packed_size(&auth_parcel);
  jnx_uint8 *obuffer = malloc(parcel_len);
  auth_receiver__pack(&auth_parcel,obuffer);

  free(auth_parcel.receiver_guid);
  free(auth_parcel.receiver_public_key);

  *onetbuffer = obuffer;
  return parcel_len;
}
int protobuf_construction_generate_public_key_request(session *ses,\
    jnx_uint8 *initiator_message,
    jnx_uint8 **onetbuffer) {
  return protobuf_construction_initiator_command_generate(ses,
      CHALLENGE_PUBLIC_KEY,NULL,0,initiator_message,
      onetbuffer);
}
int protobuf_construction_generate_finish_request(session *ses,\
    jnx_uint8 *shared_secret,jnx_size len,
    jnx_uint8 **onetbuffer) {
  return protobuf_construction_initiator_command_generate(ses,
      CHALLENGE_FINISH,shared_secret,len,NULL,onetbuffer);
}
int protobuf_construction_generate_public_key_response(session *ses,\
    jnx_int abort,
    jnx_uint8 **onetbuffer) {
  return protobuf_construction_receiver_command_generate(ses,
      RESPONSE_PUBLIC_KEY,abort,onetbuffer);
}
int protobuf_construction_generate_finish_response(session *ses,\
    jnx_int abort,
    jnx_uint8 **onetbuffer) {
  return protobuf_construction_receiver_command_generate(ses,
      RESPONSE_FINISH,abort,onetbuffer);
}
int protobuf_construction_invite_command_generate(session *ses,
    jnx_guid *invitee_guid, jnx_uint8 **onetbuffer) {

  AuthInvite auth_invite = AUTH_INVITE__INIT;
  jnx_char *local_guid_str;
  jnx_guid_to_string(&(*ses).local_peer_guid,&local_guid_str);
  jnx_size len = strlen(local_guid_str);
  /* inviter guid */
  auth_invite.inviter_guid = malloc(sizeof(char) * len + 1);
  memcpy(auth_invite.inviter_guid,local_guid_str,len + 1); 
  free(local_guid_str);
  /* session guid */
  jnx_char *session_guid_str;
  jnx_guid_to_string(&(*ses).session_guid,&session_guid_str);
  len = strlen(session_guid_str);
  auth_invite.session_guid = malloc(sizeof(char)* len + 1);
  memcpy(auth_invite.session_guid,session_guid_str, len + 1);
  free(session_guid_str);
  /*invitee guid */
  jnx_char *invitee_guid_str;
  jnx_guid_to_string(invitee_guid,&invitee_guid_str);
  len = strlen(invitee_guid_str);
  auth_invite.invitee_guid = malloc(sizeof(char) * len + 1);
  memcpy(auth_invite.invitee_guid,invitee_guid_str,len + 1);

  jnx_int parcel_len = auth_invite__get_packed_size(&auth_invite);
  jnx_uint8 *obuffer = malloc(parcel_len);
  auth_invite__pack(&auth_invite,obuffer);

  free(auth_invite.inviter_guid);

  *onetbuffer = obuffer;
  return parcel_len;
}
int protobuf_construction_generate_invite_request(session *ses,
    jnx_guid *invitee_guid, jnx_uint8 **onetbuffer) {
  return protobuf_construction_invite_command_generate(ses,
      invitee_guid,onetbuffer);
}
int protobuf_construction_joiner_command_generate(session *ses, \
    protobuf_construction_joiner_state state, jnx_uint8 *encrypted_joiner_guid,\
    jnx_size len,
    jnx_uint8 **onetbuffer) {

  AuthJoiner auth_joiner = AUTH_JOINER__INIT;  
  /* session guid */
  jnx_char *session_guid_str;
  jnx_guid_to_string(&(*ses).session_guid,&session_guid_str);
  len = strlen(session_guid_str);
  auth_joiner.session_guid = malloc(sizeof(char)* len + 1);
  memcpy(auth_joiner.session_guid,session_guid_str, len + 1);
  free(session_guid_str);
  /* encrypted joiner guid */
  
  auth_joiner.encrypted_joiner_guid.len = len;
  auth_joiner.encrypted_joiner_guid.data = malloc(sizeof(char) * len +1);  
  
  jnx_int i;
  for(i=0;i<auth_joiner.encrypted_joiner_guid.len; ++i) {
    auth_joiner.encrypted_joiner_guid.data[i] = encrypted_joiner_guid[i];
  }
  
  jnx_int parcel_len = auth_joiner__get_packed_size(&auth_joiner);
  jnx_uint8 *obuffer = malloc(parcel_len);
  auth_joiner__pack(&auth_joiner,obuffer);
  /* requesting join */
  auth_joiner.is_requesting_join = 1;
  *onetbuffer = obuffer;

  return parcel_len;
}
int protobuf_construction_generate_joiner_request(session *ses, \
    jnx_uint8 *encrypted_joiner_guid, 
    jnx_size len,jnx_uint8 **onetbuffer) {
  return protobuf_construction_joiner_command_generate(ses, JOINER_JOIN, 
      encrypted_joiner_guid, len, onetbuffer);
}
int protobuf_construction_secure_comms_generate(session *ses,
    jnx_char *message, jnx_uint8 **onetbuffer) {

  SecureCommsObject sco = SECURE_COMMS_OBJECT__INIT;

  jnx_char *session_guid_str;
  jnx_guid_to_string(&(*ses).session_guid,&session_guid_str);
  jnx_int len = strlen(session_guid_str);
  sco.session_guid = malloc(sizeof(char)* len + 1);
  memcpy(sco.session_guid,session_guid_str, len + 1);
  free(session_guid_str);
  
  sco.message = malloc(strlen(message) + 1);
  memcpy(sco.message,message,strlen(message));
  
  jnx_int olen = secure_comms_object__get_packed_size(&sco);
  jnx_uint8 *obuffer = malloc(olen);
  secure_comms_object__pack(&sco,obuffer);

  *onetbuffer = obuffer;
  free(sco.message);
  free(sco.session_guid);

  return olen;
}
