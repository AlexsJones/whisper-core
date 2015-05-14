#include "port_control.h"
#include <stdlib.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include <string.h>

jnx_int is_port_available(jnx_int port) {
  JNXCHECK(port > 0);
  struct addrinfo hints, *res, *p;
  jnx_int32 sock = socket(AF_INET,SOCK_STREAM,0);
  jnx_int32 optval = 1;
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  jnx_char sport[46]={};
  sprintf(sport,"%d",port);
  getaddrinfo(NULL,sport,&hints,&res);
  p = res;
  while(p != NULL) {
    if (setsockopt(sock,
          SOL_SOCKET,
          SO_REUSEADDR,
          &optval,sizeof(jnx_int32)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    if(bind(sock,p->ai_addr,p->ai_addrlen) == -1) {
      freeaddrinfo(res);
      return 0;
    }
    break;
    p = p->ai_next;
  }
  freeaddrinfo(res);
  close(sock);
  return 1;
}
jnx_int port_control_service_scan_range(port_control_service *p,E_SCAN_STATE
    state) {
  jnx_int x;
  for(x=p->crange;x<(p->urange + 1);x=x+p->interval) {
    JNXLOG(LDEBUG,"Testing port %d [%d:%d]",x,p->crange,p->urange);
    jnx_int is_available = is_port_available(x);
    if(is_available) {
      return x;
    }
  }
  return -1;
}
jnx_int port_control_service_next_available(port_control_service *p) {
  jnx_int port =  port_control_service_scan_range(p,1);
  p->crange = port + 1;
  if(p->crange >= p->urange +1)  {
    p->crange = p->lrange;
  }
  return port;
}
jnx_char* port_control_service_next_available_to_s(port_control_service *p) {
  jnx_char buffer[46]={};
  jnx_int port = port_control_service_next_available(p);
  sprintf(buffer,"%d",port);
  return strdup(buffer);
}
port_control_service *port_control_service_create(jnx_int lrange, 
    jnx_int urange, jnx_int interval) {
  JNXCHECK(lrange > 0);
  JNXCHECK(urange > lrange);
  JNXCHECK(interval > 0);
  port_control_service *p = malloc(sizeof(port_control_service));
  p->lrange = lrange;
  p->crange = p->lrange;
  p->urange = urange;
  p->interval = interval;
  return p;
}
void port_control_service_destroy(port_control_service **p){
  free(*p);
  *p = NULL;
}
