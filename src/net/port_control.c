#include "port_control.h"
#include <stdlib.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include <string.h>

jnx_int is_port_available(jnx_int port) {
  JNXCHECK(port > 0);
  struct addrinfo hints, *res, *p;
  jnx_int32 sock = socket(AF_INET,SOCK_STREAM,0);
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  jnx_char sport[46]={};
  sprintf(sport,"%d",port);
  getaddrinfo(NULL,sport,&hints,&res);
  p = res;
  while(p != NULL) {
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
jnx_int port_control_scan_range(port_control *p,E_SCAN_STATE
    state) {
  jnx_int x;
  for(x=p->lrange;x<(p->urange + 1);x=x+p->interval) {
    JNX_LOG(NULL,"Testing port %d [%d:%d]",x,p->lrange,p->urange);
    jnx_int is_available = is_port_available(x);
    if(is_available) {
      return x;
    }
  }
  return -1;
}
jnx_int port_control_next_available(port_control *p) {
  return port_control_scan_range(p,1);
}
jnx_char* port_control_next_available_to_s(port_control *p) {
  jnx_char buffer[46]={};
  jnx_int port = port_control_next_available(p);
  sprintf(buffer,"%d",port);
  return strdup(buffer);
}
port_control *port_control_create(jnx_int lrange, 
    jnx_int urange, jnx_int interval) {
  JNXCHECK(lrange > 0);
  JNXCHECK(urange > lrange);
  JNXCHECK(interval > 0);
  port_control *p = malloc(sizeof(port_control));
  p->lrange = lrange;
  p->urange = urange;
  p->interval = interval;
  return p;
}
void port_control_destroy(port_control **p){
  free(*p);
  *p = NULL;
}
