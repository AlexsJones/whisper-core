#include "port_control.h"
#include <stdlib.h>
#include <jnxc_headers/jnx_tcp_socket.h>

jnx_int is_port_available(jnx_char* port) {
  struct addrinfo hints, *res, *p;
  jnx_int32 sock = socket(AF_INET,SOCK_STREAM,0);
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(NULL,port,&hints,&res);
  p = res;
  while(p != NULL) {
    if(bind(sock,p->ai_addr,p->ai_addrlen) == -1) {
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
  for(x=p->lrange;x<p->urange;x=x+p->interval) {
    jnx_char buf[64] ={};
    sprintf(buf,"%d",x);
    jnx_int is_available = is_port_available(buf);
    if(is_available) {
      if(state == RETURN_EARLY_SCAN) {
        return x;
      }
      /* TODO: */
    }
  }
  return 0;
}
jnx_int port_control_next_available(port_control *p) {
  return port_control_scan_range(p,1);
}
jnx_char* port_control_next_available_to_s(port_control *p) {

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
}
void port_control_destroy(port_control **p){
	free(*p);
	*p = NULL;
}
