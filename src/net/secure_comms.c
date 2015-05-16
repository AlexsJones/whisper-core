/*
 * secure_comms.c
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */

#include "secure_comms.h"
#include <jnxc_headers/jnxsocket.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <jnxc_headers/jnxthread.h>
#include <jnxc_headers/jnx_tcp_socket.h>

jnx_int secure_comms_is_socket_linked(jnx_int sock) {
  if(sock == -1) {
    return 0;
  }
  jnx_int error = 0;
  socklen_t len = sizeof(error);
  jnx_int retval = getsockopt(sock,SOL_SOCKET,SO_ERROR,&error,&len);
  if(retval == 0) {
    return 1;
  }
  return 0;
}
int listen_for_socket_fd(peer *remote_peer,session *ses) {
  jnx_int32 sock = socket(AF_INET,SOCK_STREAM,0);
  jnx_int32 optval = 1;
  struct addrinfo hints, *res, *p;
  struct sockaddr_storage their_addr;
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  JNXLOG(0,"listen_for_socket_fd: getaddrinfo");

  JNXCHECK(getaddrinfo(NULL,ses->secure_comms_port,&hints,&res) == 0);
  p = res;
  while(p != NULL) {
    if (setsockopt(sock,
          SOL_SOCKET,
          SO_REUSEADDR,
          &optval,sizeof(jnx_int32)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    JNXLOG(0,"listen_for_socket_fd: bind");
    if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
      perror("server:");
      return -1;
    }
    break;
    p = p->ai_next;
  }
  freeaddrinfo(res);
  JNXLOG(0,"listen_for_socket_fd: listen");
  listen(sock,10);
  socklen_t addr_size = sizeof(their_addr);
  JNXLOG(0,"listen_for_socket_fd: accept");
  int fd = accept(sock,(struct sockaddr*)&their_addr,&addr_size);
  if(fd < 0) {
    perror("accept:");
    return -1;
  }
  ses->is_connected = 1;
  close(sock);
  return fd;
}
int connect_for_socket_fd(peer *remote_peer,session *ses) {

  jnx_int32 sock = socket(AF_INET,SOCK_STREAM,0);
  struct addrinfo hints, *res;  
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  JNXLOG(0,"connect_for_socket_fd: getaddrinfo");
  jnx_int32 rg = 0;
  if((rg = getaddrinfo(remote_peer->host_address,
          ses->secure_comms_port,&hints,&res)) != 0) {

    JNXLOG(LDEBUG,"%s\n",gai_strerror(rg));
    return -1;
  }

  if(connect(sock,res->ai_addr,res->ai_addrlen) != 0) {
    perror("connect");
    freeaddrinfo(res);
    return -1;
  }
  ses->is_connected = 1;
  JNXLOG(0,"connect_for_socket_fd: ses->is_connected connected!");
  freeaddrinfo(res);
  return sock;
}
void secure_comms_end(session *s) {
  if(s->secure_socket) {
    JNXLOG(LDEBUG,"Secure comms shutting down fd %d",s->secure_socket);
    if(shutdown(s->secure_socket,SHUT_RDWR) != 0) {
      perror("socket shutdown: ");
    }
    s->secure_socket = -1;
  }
}
jnx_int secure_comms_start(secure_comms_endpoint e, discovery_service *ds,
    session *s,jnx_unsigned_int addr_family) {
  JNXCHECK(s);
  JNXCHECK(s->is_connected);
  printf("Starting secure comms on %s.\n",s->secure_comms_port);

  peer *local_peer = peerstore_get_local_peer(ds->peers);
  JNXCHECK(local_peer);
  peer *remote_peer = peerstore_lookup(ds->peers,&(*s).remote_peer_guid);
  JNXCHECK(remote_peer);
  printf("Starting a tunnel to %s\n",remote_peer->host_address);


  JNXCHECK(s->secure_socket == -1);

  switch(e) {

    case SC_INITIATOR:
      printf("About to initiate connection to remote secure_comms_port.\n");
      sleep(3);
      s->secure_socket = connect_for_socket_fd(remote_peer,s);
      printf("Secure socket fd: %d\n",s->secure_socket);
      break;

    case SC_RECEIVER:
      printf("Setting up recevier.\n");
      s->secure_socket = listen_for_socket_fd(remote_peer,s);
      printf("Secure socket fd: %d\n",s->secure_socket);
      break;
  }

  JNXCHECK(s->secure_socket != -1);
  return s->secure_socket;
}
jnx_int secure_comms_receiver_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family) {
  return secure_comms_start(SC_RECEIVER,ds,s,addr_family);
}
jnx_int secure_comms_initiator_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family) {
  return secure_comms_start(SC_INITIATOR,ds,s,addr_family);
}
