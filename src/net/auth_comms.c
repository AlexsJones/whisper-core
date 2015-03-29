/*
 * session_service_auth_comms.c
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxencoder.h>
#include "auth_comms.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "cryptography.h"
#include "utils.h"
#include "auth_initiator.pb-c.h"
#include "auth_receiver.pb-c.h"
#include "handshake_control.h"
#include "secure_comms.h"
#define CHALLENGE_REQUEST_PUBLIC_KEY 1
#define CHALLENGE_REQUEST_FINISH 0

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
  printf("awaiting reply from %s\n",hostname);
  *receipt_bytes = jnx_socket_tcp_send_with_receipt(sock,hostname,port,buffer,\
      bytes,&reply);
  jnx_socket_destroy(&sock);
  return reply;
}
static void listener_callback(const jnx_uint8 *payload,
    jnx_size bytes_read, int connected_socket, void *context) {

  transport_options *t = (transport_options*)context;
  void *object;
  int abort_token = 0;
  if(handshake_did_receive_initiator_request((jnx_char*)payload,bytes_read,&object)) {
    AuthInitiator *a = (AuthInitiator*)object;
    /*
     *At this point the receiver does not have a session for PeerA/B
     *We'll need to insert one so the session reference is usable in the ongoing
     *comms
     */
    if(a->is_requesting_public_key && !a->is_requesting_finish){

      jnx_guid g, session_g;
      jnx_guid_from_string(a->initiator_guid,&g);
      jnx_guid_from_string(a->session_guid,&session_g);

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
      session_service_link_sessions(t->ss,
      t->linking_args,&session_g, local_peer, remote_peer);

      printf("Created a linked session with the local peer %s and remote peer %s\n",
        local_peer->user_name,remote_peer->user_name);
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
      printf("Did receive encrypted shared secret.\n"); 
      session *osession;
      jnx_guid g;
      jnx_guid_from_string(a->session_guid,&g);
      session_service_fetch_session(t->ss,
          &g,&osession);

      jnx_uint8 *onetbuffer;
      int bytes = handshake_generate_finish_response(osession,abort_token,
          &onetbuffer);
      write(connected_socket,onetbuffer,bytes);
      free(onetbuffer);    

      /* The last thing to do is to decrypt the shared secret and store it in
       * the session */

      jnx_size olen;
      jnx_size decoded_len;
      jnx_encoder *encoder = jnx_encoder_create();
      jnx_uint8 *decoded_secret = 
        jnx_encoder_b64_decode(encoder,a->shared_secret,
            strlen(a->shared_secret),&decoded_len);

      jnx_char *decrypted_shared_secret = 
        asymmetrical_decrypt(osession->keypair,decoded_secret,
            decoded_len,
            &olen);
      //DEBUG ONLY
#ifndef RELEASE
      printf("DEBUG => shared secret:%s\n",decrypted_shared_secret);
      printf("DEBUG => secure_comms_port:%s\n",osession->secure_comms_port);
#endif
      session_add_shared_secret(osession,decrypted_shared_secret);

      osession->is_connected = 1;
      printf("Handshake complete.\n");
      printf("Starting secure comms channel.\n");
      secure_comms_receiver_start(t->ds,osession,t->ac->listener->socket->addrfamily);
      /* free data */
      jnx_encoder_destroy(&encoder);
      auth_initiator__free_unpacked(a,NULL);
      return;
    }
  } 
  return;
}
static void *listener_bootstrap(void *args) {
  transport_options *t = (transport_options*)args;
  while(1) {
    jnx_socket_tcp_listener_tick(t->ac->listener,
      listener_callback,t);
  }
}
auth_comms_service *auth_comms_create() {
  return malloc(sizeof(auth_comms_service));
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
  jnx_socket_tcp_listener_destroy(&(*ac)->listener);
}
void auth_comms_initiator_start(auth_comms_service *ac, \
    discovery_service *ds, session *s, jnx_char *secure_port) {

  if(s->is_connected) {

    printf("This session is already connected.\n");
    return;
  }
  session_add_secure_comms_port(s,secure_port);

  peer *remote_peer = peerstore_lookup(ds->peers,&(*s).remote_peer_guid);
  JNXCHECK(remote_peer);

  jnx_uint8 *obuffer;
  jnx_int bytes_read = handshake_generate_public_key_request(s,&obuffer);

  printf("Generated initial handshake...[%d/bytes]\n",bytes_read);
  jnx_size replysize;
  jnx_uint8 *reply = send_data_await_reply(remote_peer->host_address,"9991", 
      ac->listener->socket->addrfamily,
      obuffer,bytes_read,&replysize);

  /* expect an AuthReceiver public key reply here */
  void *object;
  printf("Received reply on initial handshake.\n");
  if(handshake_did_receive_receiver_request(reply,replysize,&object)) {
    AuthReceiver *r = (AuthReceiver *)object;
 
    /* first thing we check is if we should abort */
    if(r->should_abort) {
      printf("Handshake has been rejected.\n");
      free(obuffer);
      auth_receiver__free_unpacked(r,NULL);
      return;
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
    jnx_encoder *encoder = jnx_encoder_create();
    jnx_uint8 *encoded_secret = jnx_encoder_b64_encode(encoder,
        encrypted_secret,encrypted_secret_len,
        &encoded_len);
    /*                                                            */

    jnx_uint8 *fbuffer;
    bytes_read = handshake_generate_finish_request(s,encoded_secret,
        encoded_len,&fbuffer);

    jnx_size replysizetwo;
    jnx_uint8 *replytwo = send_data_await_reply(remote_peer->host_address,"9991", 
        ac->listener->socket->addrfamily,
        fbuffer,bytes_read,&replysizetwo);

    /* free data */
    jnx_encoder_destroy(&encoder);
    asymmetrical_destroy_key(remote_pub_keypair);
    free(encrypted_secret);
    free(encoded_secret);
    free(reply);
    free(secret);
    free(fbuffer);
    free(obuffer);
    auth_receiver__free_unpacked(r,NULL);

    void *finish_object;
    printf("Checking object type 2.\n");
    if(handshake_did_receive_receiver_request(replytwo,replysizetwo,
          &finish_object)){
      printf("Unpacked auth receiver request\n");
      AuthReceiver *ar = (AuthReceiver *)finish_object;
      if(ar->is_receiving_finish == 1 && ar->is_receiving_public_key == 0) {
        s->is_connected = 1;
        printf("Handshake complete.\n");
        secure_comms_initiator_start(ds,s,ac->listener->socket->addrfamily);
      }
      /* free data */
      free(replytwo);
      auth_receiver__free_unpacked(ar,NULL);
    }
  }
}

