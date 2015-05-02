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
  return send(sock,"PING",4,0) ? 1 : 0;
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

  JNX_LOG(0,"listen_for_socket_fd: getaddrinfo");

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
    JNX_LOG(0,"listen_for_socket_fd: bind");
    if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
      perror("server: bind");
      return -1;
    }
    break;
    p = p->ai_next;
  }
  freeaddrinfo(res);
  JNX_LOG(0,"listen_for_socket_fd: listen");
  listen(sock,1);
  socklen_t addr_size = sizeof(their_addr);
  JNX_LOG(0,"listen_for_socket_fd: accept");
  int fd = accept(sock,(struct sockaddr*)&their_addr,&addr_size);
  if(fd > 0) {
    perror("accept:");
    return -1;
  }
  ses->is_connected = 1;
  return fd;
}
int connect_for_socket_fd(peer *remote_peer,session *ses) {

  jnx_int32 sock = socket(AF_INET,SOCK_STREAM,0);
  struct addrinfo hints, *res;  
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  JNX_LOG(0,"connect_for_socket_fd: getaddrinfo");
  jnx_int32 rg = 0;
  if((rg = getaddrinfo(remote_peer->host_address,
    ses->secure_comms_port,&hints,&res)) != 0) {

    JNX_LOG(DEFAULT_CONTEXT,"%s\n",gai_strerror(rg));
    return -1;
  }
  JNX_LOG(0,"connect_for_socket_fd: before ses->is_connected connect");
  if(!ses->is_connected) {
    JNX_LOG(0,"connect_for_socket_fd: ses->is_connected connect");
    if(connect(sock,res->ai_addr,res->ai_addrlen) != 0) {
      perror("connect");
      freeaddrinfo(res);
      return -1;
    }
    ses->is_connected = 1;
    JNX_LOG(0,"connect_for_socket_fd: ses->is_connected connected!");
  }

  printf("--------->%d\n",__LINE__);
  freeaddrinfo(res);
  return sock;
}
void secure_comms_end(session *s) {
  if(s->secure_socket) {
    jnx_socket_destroy(&(*s).secure_socket);
  }
}
jnx_socket* secure_comms_start(secure_comms_endpoint e, discovery_service *ds,
    session *s,jnx_unsigned_int addr_family) {
  JNXCHECK(s);
  JNXCHECK(s->is_connected);
  printf("Starting secure comms on %s.\n",s->secure_comms_port);

  peer *local_peer = peerstore_get_local_peer(ds->peers);
  JNXCHECK(local_peer);
  peer *remote_peer = peerstore_lookup(ds->peers,&(*s).remote_peer_guid);
  JNXCHECK(remote_peer);
  printf("Starting a tunnel to %s\n",remote_peer->host_address);

  if(s->secure_socket != NULL) {
    JNX_LOG(NULL,"Found an existing secure comms socket, closing now");
    secure_comms_end(s);
  }
  
  s->secure_socket = -1;

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

printf("--------->%d\n",__LINE__);
  JNXCHECK(s->secure_socket != -1);

  return s->secure_socket;
}
jnx_socket* secure_comms_receiver_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family) {
  return secure_comms_start(SC_RECEIVER,ds,s,addr_family);
}
jnx_socket* secure_comms_initiator_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family) {
  return secure_comms_start(SC_INITIATOR,ds,s,addr_family);
}
