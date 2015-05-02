/*
 * =====================================================================================
 *
 *       Filename:  test_session.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/09/2015 05:57:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnx_tcp_socket.h>
#include "session_service.h"
#include "secure_comms.h"
#include "discovery.h"
static char *baddr = NULL;
static int linking_did_use_functor=0;
static int unlinking_did_use_functor=0;
static int connector_sockfd=0;
static int is_receiver = 0;
typedef struct dtostr {
  peer *p;
  session *sess;
  char *port;
}dtostr;

void *worker(void *args) {
  sleep(2);
  dtostr *d = (dtostr*)args;
  connector_sockfd = connect_for_socket_fd(d->p,d->sess);
}
void fire_threaded_tcp_packet(char *port) {
  jnx_thread_create_disposable(worker,port);
}
int linking_test_procedure(session *s,linked_session_type session_type, void *optargs) {
  JNX_LOG(NULL,"Session hit linking procedure functor");
  linking_did_use_functor=1;
  if(session_type == E_AM_RECEIVER) {
    is_receiver = 1;
  }
  return 0;
}
int unlinking_test_procedure(session *s,linked_session_type session_type, void *optargs) {
  JNX_LOG(NULL,"Session hit unlinking procedure functor");
  const char *arg = (const char*)optargs;
  JNXCHECK(strcmp(arg,"PASSED") == 0);
  unlinking_did_use_functor=1;
  return 0;
}
void test_secure_comms_setup_link() {
  JNX_LOG(NULL,"test_secure_comms_setup_link");
  session_service *service = session_service_create(linking_test_procedure,
      unlinking_test_procedure);
  session *os;
  session_state e = session_service_create_session(service,&os);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 0);
  //Lets generate the guid of some remote session
  jnx_guid h;
  jnx_guid g;
  jnx_guid_create(&h);
  jnx_guid_create(&g);

  peer *l = peer_create(h,"127.0.0.1","Alex", 10);
  peer *n = peer_create(g,"127.0.0.1","Bob", 10);
  e = session_service_link_sessions(service,
      E_AM_RECEIVER,NULL,
      &os->session_guid,l,n);
  JNXCHECK(e == SESSION_STATE_OKAY);
  JNXCHECK(linking_did_use_functor);
  JNXCHECK(is_receiver == 1);
  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 1);

  e = session_service_unlink_sessions(service,0,
      "PASSED",
      &os->session_guid);

  JNXCHECK(session_service_session_is_linked(service,&os->session_guid) == 0);

  session_service_destroy(&service);
}
int main(int argc, char **argv) {
  test_secure_comms_setup_link();
  return 0;
}
