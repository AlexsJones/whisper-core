/*********************************************************************************
 *     File Name           :     test/unit_test/test_port_control_service.c
 *     Created By          :     tibbar
 *     Creation Date       :     [2015-04-03 05:30]
 *     Last Modified       :     [2015-04-03 22:48]
 *     Description         :      
 **********************************************************************************/
#include "port_control.h"
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnx_tcp_socket.h>

#define DEFAULT_SINGLE_PORT 9013
#define DEFAULT_SINGLE_PORT_NEXT 9014
void test_single_port() {
  int is_available = is_port_available(DEFAULT_SINGLE_PORT);
  JNXCHECK(is_available == 1);
}
void test_port_control_service_scan() {
  port_control_service *p = port_control_service_create(8001,9010,2);
  port_control_service_scan_range(p,FULL_SCAN);
  port_control_service_destroy(&p);
}
void test_port_control_service_next_available() {
  int is_available = is_port_available(DEFAULT_SINGLE_PORT);
  JNXCHECK(is_available == 1);

  port_control_service *p = port_control_service_create(DEFAULT_SINGLE_PORT,
      DEFAULT_SINGLE_PORT_NEXT,1);

  jnx_char *s = port_control_service_next_available_to_s(p);

  JNXCHECK(strcmp("9013",s)==0);

  port_control_service_destroy(&p);
}
int bind_socket_to_port(jnx_int port) {
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
      return 0;
    }
    break;
    p = p->ai_next;
  }
  freeaddrinfo(res);
  return sock;
}
void test_port_use_sequential() {

  port_control_service *p = port_control_service_create(6000,6001,1);
  
  jnx_int next = port_control_service_next_available(p);

  jnx_int sockfd = bind_socket_to_port(next);

  next = port_control_service_next_available(p);
  
  jnx_int socktwo = bind_socket_to_port(next);

  next = port_control_service_next_available(p);
  
  JNXCHECK(next == -1); /* ran out of ports */
  
  close(sockfd);
  close(socktwo);
  
  port_control_service_destroy(&p);
}
void test_port_increment() {
  
  port_control_service *p = port_control_service_create(6000,6001,1);
  
  jnx_int next = port_control_service_next_available(p);
  
  JNXCHECK(next == 6000);
  
  next = port_control_service_next_available(p);
  
  JNXCHECK(next == 6001);
  
  next = port_control_service_next_available(p);
  
  JNXCHECK(next == 6000);

  port_control_service_destroy(&p);
}
int main(int argc, char **argv) {
  JNXLOG(NULL,"Test port control service");
  test_single_port();
  JNXLOG(NULL,"Test port control scanning");
  test_port_control_service_scan();
  JNXLOG(NULL,"Test port control scan next available");
  test_port_control_service_next_available();
  JNXLOG(NULL,"Test using ports sequentially");
  test_port_use_sequential();
  JNXLOG(NULL,"Test port increment");
  test_port_increment();
  return 0;
}

