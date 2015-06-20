/*
 * session_service_auth_comms.c
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */
#include <jnxc_headers/jnxlog.h>
#include "auth_comms.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "cryptography.h"
#include "utils.h"
#include "auth_initiator.pb-c.h"
#include "auth_receiver.pb-c.h"
#include "auth_joiner.pb-c.h"
#include "auth_invite.pb-c.h"
#include "handshake_control.h"
#include "secure_comms.h"
#define CHALLENGE_REQUEST_PUBLIC_KEY 1
#define CHALLENGE_REQUEST_FINISH 0
#define DEFAULT_AUTH_COMMS_PORT "9991"
typedef struct transport_options {
  discovery_service *ds;
  session_service *ss;
  auth_comms_service *ac;
  void *linking_args;
}transport_options;
/* Listener Thread */
static void send_data(jnx_char *hostname, jnx_char *port,
    unsigned int family,
    jnx_uint8 *buffer, jnx_int bytes) {
  jnx_socket *sock = jnx_socket_tcp_create(family);
  jnx_socket_tcp_send(sock,hostname,port,buffer,bytes);
  jnx_socket_destroy(&sock);
}
static jnx_uint8 *send_data_await_reply(jnx_char *hostname, jnx_char *port,
    unsigned int family, jnx_uint8 *buffer, 
    jnx_int bytes, jnx_size *receipt_bytes) {
  jnx_socket *sock = jnx_socket_tcp_create(family);
  jnx_uint8 *reply;
  JNXLOG(LDEBUG,"Awaiting reply from %s",hostname);
  *receipt_bytes = jnx_socket_tcp_send_with_receipt(sock,hostname,port,buffer,\
      bytes,&reply);
  JNXLOG(LDEBUG,"Reply received");
  jnx_socket_destroy(&sock);
  return reply;
}
static void internal_start_secure_comms_initiator(discovery_service *ds, session *s,
    auth_comms_service *ac){
  secure_comms_initiator_start(ds,s,ac->listener->socket->addrfamily);
}
static void internal_start_secure_comms_listener(discovery_service *ds,
    session *s, auth_comms_service *ac) {
  secure_comms_receiver_start(ds,s,
      ac->listener->socket->addrfamily);
}
static void internal_request_initiator(transport_options *t,
    const jnx_uint8 *payload,
    jnx_size bytes_read, int connected_socket, void *object,void *context) {
  int abort_token = 0; 
  AuthInitiator *a = (AuthInitiator*)object;
  /*
   *At this point the receiver does not have a session for PeerA/B
   *We'll need to insert one so the session reference is usable in the ongoing
   *comms
   */
  if(a->is_requesting_public_key && !a->is_requesting_finish){
    JNXLOG(LDEBUG,"--------------Initial request---------------------------------");
    jnx_guid g, session_g;
    jnx_guid_from_string(a->initiator_guid,&g);
    jnx_guid_from_string(a->session_guid,&session_g);
    JNXCHECK(t->ac->ar_callback);
    abort_token = t->ac->ar_callback(t->ds,&g,&session_g);

    printf("Did receive handshake request.\n");
    session *osession;
    session_state e = session_service_create_shared_session(t->ss,
        a->session_guid,&osession);
    /* First thing we'll do is link sessions */
    printf("Created shared session\n");
    peer *local_peer = peerstore_get_local_peer(t->ds->peers);
    JNXCHECK(local_peer);
    printf("Got local peer\n");
    peer *remote_peer = peerstore_lookup(t->ds->peers,&g);
    JNXCHECK(remote_peer);
    printf("Got remote peer\n");
    session_service_link_sessions(t->ss,0,
        t->linking_args,&session_g, local_peer, remote_peer);

    printf("Created a linked session with the local peer %s and remote peer %s\n",
        local_peer->user_name,remote_peer->user_name);

    if(a->initiator_message) {
      JNXLOG(LINFO,"The incoming session says: %s\n",a->initiator_message);
      session_add_initiator_message(osession,a->initiator_message);
    }
    /* setting our response key as the 'remote public key' */
    session_add_initiator_public_key(osession,a->initiator_public_key); 
    session_add_secure_comms_port(osession,a->secure_comms_port);
    printf("Generated shared session\n");
    /*
     *Now we have a session on the receiver with a matching GUID to the sender
     *We'll have a valid public key we can send over
     */
    jnx_uint8 *onetbuffer;
    printf("About to generate handshake.\n");
    int bytes = handshake_generate_public_key_response(osession,abort_token,
        &onetbuffer);
    write(connected_socket,onetbuffer,bytes);
    /* free data */
    free(onetbuffer);    
    auth_initiator__free_unpacked(a,NULL);

    if(abort_token) {
      printf("Aborting session.\n");
    }
    return;
  }
  if(!a->is_requesting_public_key && a->is_requesting_finish){
    JNXLOG(LDEBUG,"--------------Initial request part 2---------------------------------");
    printf("Did receive encrypted shared secret.\n"); 
    session *osession;
    jnx_guid g;
    jnx_guid_from_string(a->session_guid,&g);
    if(session_service_fetch_session(t->ss,
          &g,&osession) != SESSION_STATE_OKAY) {
      JNXLOG(LDEBUG,"An unknown session has attempted to initiate second stage\
          handshake");
      /* TODO: Log this attempt to access second stage handshake possible 
       * attack */
      return;
    } 

    jnx_uint8 *onetbuffer;
    int bytes = handshake_generate_finish_response(osession,abort_token,
        &onetbuffer);
    write(connected_socket,onetbuffer,bytes);
    free(onetbuffer);    

    /* The last thing to do is to decrypt the shared secret and store it in
     * the session */
    jnx_size olen;
    jnx_size decoded_len;
    jnx_uint8 *decoded_secret = 
      jnx_encoder_b64_decode(t->ac->encoder,a->shared_secret,
          strlen(a->shared_secret),&decoded_len);

#ifdef DEBUG
    JNXLOG(LDEBUG,"The encoder decoded length is %d with raw secret %s",decoded_len,decoded_secret);
    JNXCHECK(osession->keypair);
    JNXLOG(LDEBUG,"The decoded but encrypted symmetrical secret is => %s of length %d",decoded_secret,decoded_len);
#endif  
    jnx_char *decrypted_shared_secret = 
      asymmetrical_decrypt(osession->keypair,decoded_secret,
          decoded_len,
          &olen);
    //DEBUG ONLY
#ifdef DEBUG
    printf("DEBUG => shared secret:%s\n",decrypted_shared_secret);
    printf("DEBUG => secure_comms_port:%s\n",osession->secure_comms_port);

    jnx_char *guid_to_str;
    jnx_guid_to_string(&(*osession).session_guid,&guid_to_str);
    printf("DEBUG => session guid:%s\n",guid_to_str);
    free(guid_to_str);
#endif
    session_add_shared_secret(osession,decrypted_shared_secret);

    osession->is_connected = 1;
    printf("Handshake complete.\n");
    printf("Starting secure comms channel.\n");

    internal_start_secure_comms_listener(t->ds,
        osession,t->ac);

    /* free data */
    auth_initiator__free_unpacked(a,NULL);
  }
}
static void internal_request_receiver(transport_options *t,
    const jnx_uint8 *payload,
    jnx_size bytes_read, int connected_socket, void *object,void *context) {
}
static void internal_request_invite(transport_options *t,
    const jnx_uint8 *payload,
    jnx_size bytes_read, int connected_socket, void *object, void *context) {
  JNXLOG(LDEBUG,"handshake_generate_invite_request"); 
  AuthInvite *i = (AuthInvite*)object;
  JNXLOG(LDEBUG,"Auth invite from %s to %s is that you?",
      i->session_guid,i->invitee_guid);

  jnx_guid session_guid;
  jnx_guid_from_string(i->session_guid,&session_guid);

  peer *local_peer = peerstore_get_local_peer(t->ds->peers);
  jnx_char *local_peer_guid;
  jnx_guid_to_string(&(*local_peer).guid,&local_peer_guid);

  if(strcmp(local_peer_guid,i->invitee_guid) == 0) {
    JNXLOG(LDEBUG,"Local peer matches invitee!");
    JNXCHECK(t->ac->invitation_callback);

    /* Are we already aware of the potential session locally? */
    session *osession;
    session_state e = session_service_fetch_session(t->ss,
        &session_guid,&osession);
    if(e != SESSION_STATE_OKAY) {

      jnx_int invite_token = t->ac->invitation_callback(&session_guid); 

      if(!invite_token)  {
        JNXLOG(LWARN,"handshake_generate_invite_request has been rejected!"); 
      }else {
        /* The invite has been accepted */    
        JNXLOG(LDEBUG,"Invite from %s has been accepted",i->session_guid);

        /* Start auth join */
        //TODO: I'll do this on the same listener thread which will cause a 
        //temporary block


      }
    }else {
      JNXLOG(LWARN,"Session is already known - we don't need an invite");
    }
  }
  auth_invite__free_unpacked(i,NULL);
  free(local_peer_guid);
}
static void internal_request_joiner(transport_options *t,
    const jnx_uint8 *payload,
    jnx_size bytes_read, int connected_socket, void *object, void *context) {
  AuthJoiner *j = (AuthJoiner*)object; 
  /*
   *Any joiner must specify a valid session that the current Peer is a part of
   */
  if(j->is_requesting_join) {
    jnx_guid g;
    jnx_guid_from_string(j->session_guid,&g);
    session *osession;
    session_state e = session_service_fetch_session(t->ss,&g,
        &osession);
    if(e == SESSION_STATE_OKAY) {

      /*
       *Okay you want to join our chat.
       *First you and I will need to handshake
       *
       */

    }else {
      JNXLOG(LWARN,"There was a problem requesting the session for the auth joiner");
    }
  }

  auth_joiner__free_unpacked(j,NULL);

}
static void listener_callback(const jnx_uint8 *payload,
    jnx_size bytes_read, int connected_socket, void *context) {

  transport_options *t = (transport_options*)context;

  void *object;
  switch(handshake_resolve_request_type((jnx_char*)payload,bytes_read,&object)) {
    case REQUEST_TYPE_INITIATOR:
      JNXLOG(LDEBUG,"Received request type: Initiator");
      internal_request_initiator(t,payload,bytes_read,connected_socket,
          object,context);
      break;

    case REQUEST_TYPE_RECEIVER:
      JNXLOG(LDEBUG,"Received request type: Receiver");
      internal_request_receiver(t,payload,bytes_read,connected_socket,
          object,context);
      break;

    case REQUEST_TYPE_INVITE:
      JNXLOG(LDEBUG,"Received request type: Inviter");
      internal_request_invite(t,payload,bytes_read,connected_socket,object,
          context);
      break;

    case REQUEST_TYPE_JOINER:
      JNXLOG(LDEBUG,"Received request type: Joiner");
      internal_request_joiner(t,payload,bytes_read,connected_socket,object,
          context);
      break;
  }
}
static void *listener_bootstrap(void *args) {
  transport_options *t = (transport_options*)args;
  while(1) {
    jnx_socket_tcp_listener_tick(t->ac->listener,
        listener_callback,t);
  }
}
auth_comms_service *auth_comms_create() {
  auth_comms_service *ac = malloc(sizeof(auth_comms_service));
  ac->ar_callback = NULL;
  ac->invitation_callback = NULL;
  ac->encoder = jnx_encoder_create();
  return ac;
}
void auth_comms_listener_start(auth_comms_service *ac, discovery_service *ds,
    session_service *ss,void *linking_args) {
  transport_options *ts = malloc(sizeof(transport_options));
  ts->ac = ac;
  ts->ds = ds;
  ts->ss = ss;
  ts->linking_args = linking_args;
  ac->listener_thread = jnx_thread_create(listener_bootstrap,ts);
}
void auth_comms_destroy(auth_comms_service **ac) {
  jnx_encoder_destroy(&(*ac)->encoder);
  jnx_socket_tcp_listener_destroy(&(*ac)->listener);
}
jnx_int auth_comms_initiator_start(auth_comms_service *ac, \
    discovery_service *ds, port_control_service *ps, 
    session *s, jnx_uint8 *initiator_message) {

  if(s->is_connected) {
    printf("This session is already connected.\n");
    return 1;
  }

  jnx_char *sport = port_control_service_next_available_to_s(ps);

  session_add_secure_comms_port(s,sport);

  peer *remote_peer = peerstore_lookup(ds->peers,&(*s).remote_peer_guid);
  JNXCHECK(remote_peer);

  if(initiator_message) {
    JNXLOG(LINFO,"Setting initiator message on the session [%s]",
        initiator_message);
    session_add_initiator_message(s,initiator_message);
  }
  jnx_uint8 *obuffer;
  jnx_int bytes_read = handshake_generate_public_key_request(s,initiator_message,
      &obuffer);

  printf("Generated initial handshake...[%d/bytes]\n",bytes_read);
  jnx_size replysize;
  jnx_uint8 *reply = send_data_await_reply(remote_peer->host_address,
      DEFAULT_AUTH_COMMS_PORT, 
      ac->listener->socket->addrfamily,
      obuffer,bytes_read,&replysize);

  /* expect an AuthReceiver public key reply here */
  void *object;
  if(handshake_did_receive_receiver_request(reply,replysize,&object)) {
    AuthReceiver *r = (AuthReceiver *)object;

    /* first thing we check is if we should abort */
    if(r->should_abort) {
      printf("Handshake has been rejected.\n");
      free(obuffer);
      auth_receiver__free_unpacked(r,NULL);
      return -1;
    }
    /* At this point we have a session with the receiver public key
       we can generate the shared secret and transmit it back */
    jnx_uint8 *secret;
    jnx_size s_len = generate_shared_secret(&secret);
    /* Let's store the remote public key and our newly generate secret */
    session_add_receiver_public_key(s,r->receiver_public_key);
    session_add_shared_secret(s,secret);

    /* Lets encrypt and encode that secret before we send it back */
    RSA *remote_pub_keypair = 
      asymmetrical_key_from_string(r->receiver_public_key,PUBLIC);

    jnx_size encrypted_secret_len;
    jnx_char *encrypted_secret = asymmetrical_encrypt(remote_pub_keypair,
        secret, &encrypted_secret_len);

    jnx_size encoded_len;
    jnx_uint8 *encoded_secret = jnx_encoder_b64_encode(ac->encoder,
        encrypted_secret,encrypted_secret_len,
        &encoded_len);
    /*                                                            */

    jnx_uint8 *fbuffer;
    bytes_read = handshake_generate_finish_request(s,encoded_secret,
        encoded_len,&fbuffer);

    jnx_size replysizetwo;
    jnx_uint8 *replytwo = send_data_await_reply(remote_peer->host_address,
        DEFAULT_AUTH_COMMS_PORT, 
        ac->listener->socket->addrfamily,
        fbuffer,bytes_read,&replysizetwo);

    asymmetrical_destroy_key(remote_pub_keypair);
    free(encrypted_secret);
    free(encoded_secret);
    free(reply);
    free(secret);
    free(fbuffer);
    free(obuffer);
    auth_receiver__free_unpacked(r,NULL);

    void *finish_object;

    if(handshake_did_receive_receiver_request(replytwo,replysizetwo,
          &finish_object)){
      AuthReceiver *ar = (AuthReceiver *)finish_object;
      if(ar->is_receiving_finish == 1 && ar->is_receiving_public_key == 0) {
        s->is_connected = 1;
        printf("Handshake complete.\n");
        internal_start_secure_comms_initiator(ds,s,
            ac);
      }
      /* free data */
      free(replytwo);
      auth_receiver__free_unpacked(ar,NULL);
    }
  }
  return 0;
}
jnx_int auth_comms_invite_send(auth_comms_service *ac,
    session *s, peer *invitee) {
  JNXLOG(LDEBUG,"auth_comms_invite_send [Session guid] [Invitee]");
  print_pair(&(*s).session_guid,&(*invitee).guid); 
  JNXLOG(LDEBUG,"auth_comms_invite_send [Session guid] [Invitee]");
  jnx_uint8 *obuffer;
  jnx_int bytes_read = handshake_generate_invite_request(s,&(*invitee).guid,
      &obuffer);
  jnx_size replysize;

  send_data(invitee->host_address,
      DEFAULT_AUTH_COMMS_PORT,
      ac->listener->socket->addrfamily,
      obuffer,bytes_read);

  free(obuffer);

  return 0;
}
void auth_comms_stop(auth_comms_service *ac,session *s) {
  JNXCHECK(ac);
  JNXCHECK(s);
  secure_comms_end(s);
}
