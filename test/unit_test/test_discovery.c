/*
 * =====================================================================================
 *
 *       Filename:  test_discovery.c
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  21/01/2015 08:39:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <string.h>
#include "discovery.h"
#include "peer.h"
#include "peerstore.h"
#include <jnxc_headers/jnxcheck.h>
#include <jnxc_headers/jnxlog.h>

static char *baddr = LDEBUG;

void test_local_and_broadcast_ip() {
  char *local, *broadcast;
  get_local_ip(&local, NULL);
  get_broadcast_ip(&broadcast, NULL);
  printf("local=%s, broadcast=%s\n", local, broadcast);

  get_local_ip(&local, "vnic0");
  get_broadcast_ip(&broadcast, "vnic0");
  printf("interface (vnic0) local=%s, broadcast=%s\n", local, broadcast);

  get_local_ip(&local, "lo0");
  get_broadcast_ip(&broadcast, "lo0");
  printf("interface (lo0) local=%s, broadcast=%s\n", local, broadcast);

  get_local_ip(&local, "bob");
  get_broadcast_ip(&broadcast, "bob");
  printf("interface (bob) local=%s, broadcast=%s\n", local, broadcast);
}
void update_time_checks(discovery_service *svc) {
  time_t last_update_time = get_last_update_time(svc);
  int i;
  for (i = 0; i < 5; i++) {
    printf(".");
    fflush(stdout);
    sleep(peer_update_interval);
    time_t temp = get_last_update_time(svc);
    JNXCHECK(temp > last_update_time);
    last_update_time = temp;
  }
  printf("\n");
}

void wait_for_flag(int *flag) {
  while (!(*flag)) {
    printf(".");
    fflush(stdout);
    sleep(1);
  }
  printf("\n");
  *flag = 0;
}

void assert_valid_command(char *message) {
  char command[5];
  memcpy(command, message, 4);
  command[4] = '\0';
  JNXCHECK(strcmp(command, "PEER") == 0
           || strcmp(command, "LIST") == 0
           || strcmp(command, "STOP") == 0);
}
static int list_message_received = 0;
int starting_service_spy(char *message, int len, jnx_socket *s, void *context) {
  assert_valid_command(message);
  if (strncmp(message, "LIST", 4) == 0) {
    list_message_received = 1;
  }
  return 0;
}

// *** Discovery service test runner ***
#define CLEANUP 0
#define NO_CLEANUP 1
typedef int (*discovery_test)(discovery_service *);

void run_discovery_service_test(discovery_test test) {
  jnx_guid guid;
  int i;
  for (i = 0; i < 16; i++) {
    guid.guid[i] = i;
  }
  peerstore *store = peerstore_init(peer_create(guid, "127.0.0.1", "UserName", 10), 0);
  discovery_service *svc = discovery_service_create(1234, AF_INET, baddr, store);
#ifdef DEBUG
  printf("[DEBUG] service: %p\n", svc);
#endif
  int retval = test(svc);
  if (retval == CLEANUP) {
    discovery_service_cleanup(&svc);
  }
}

// *** Tests ***
int test_service_creation(discovery_service *svc) {
  JNXCHECK(svc != LDEBUG);
  JNXCHECK(svc->port == 1234);
  JNXCHECK(svc->family == AF_INET);
  JNXCHECK(svc->isrunning == 0);
  JNXCHECK(svc->sock_send == LDEBUG);
  JNXCHECK(svc->udp_listener == LDEBUG);
  return CLEANUP;
}
int test_service_cleanup(discovery_service *svc) {
  discovery_service_cleanup(&svc);
  JNXCHECK(svc == 0);
  return NO_CLEANUP;
}
int test_starting_service(discovery_service *svc) {
  svc->receive_callback = starting_service_spy;
  int retval = discovery_service_start(svc, ASK_ONCE_STRATEGY);
  JNXCHECK(retval == 0);
  JNXCHECK(svc->sock_send->socket > 0 
      && svc->sock_send->addrfamily == AF_INET
      && svc->sock_send->stype == SOCK_DGRAM);
  JNXCHECK(svc->udp_listener->socket > 0 
      && svc->udp_listener->socket->addrfamily == AF_INET
      && svc->udp_listener->socket->stype == SOCK_DGRAM);

  if (svc->isrunning) {
    // check send socket is up and sending
    int error = 0;
    socklen_t len = sizeof(error);
    retval = getsockopt(svc->sock_send->socket, SOL_SOCKET, SO_ERROR, &error, &len);
    JNXCHECK(retval == 0);
  }

  wait_for_flag(&list_message_received);
  return CLEANUP;
}
int test_stopping_service(discovery_service *svc) {
  test_starting_service(svc);
  discovery_service_stop(svc);
  JNXCHECK(svc->udp_listener == LDEBUG);
  JNXCHECK(svc->sock_send == 0);
  JNXCHECK(svc->isrunning == 0);
  list_message_received = 0;
  return CLEANUP;
}
int test_restarting_service(discovery_service *svc) {
  test_starting_service(svc);
  discovery_service_stop(svc);
  test_starting_service(svc);
  return CLEANUP;
}

int test_setting_peer_update_interval(discovery_service *svc) {
  JNXCHECK(peer_update_interval == 10);
  peer_update_interval = 20;
  JNXCHECK(peer_update_interval == 20);
  peer_update_interval = 10;
  JNXCHECK(peer_update_interval == 10);
  return CLEANUP;
}
int test_polling_update_strategy(discovery_service *svc) {
  peer_update_interval = 1;
  discovery_service_start(svc, POLLING_UPDATE_STRATEGY);
  sleep(2);
  update_time_checks(svc);
  return CLEANUP;
}
int test_broadcast_update_strategy(discovery_service *svc) {
  peer_update_interval = 1;
  discovery_service_start(svc, BROADCAST_UPDATE_STRATEGY);
  sleep(2);
  update_time_checks(svc);
  return CLEANUP;
}
int main(int argc, char **argv) {
  JNXLOG_CREATE("../testlogger.conf");

  test_local_and_broadcast_ip();

  get_broadcast_ip(&baddr, NULL);

  JNXLOG(LDEBUG,"Test service creation.");
  run_discovery_service_test(test_service_creation);

  JNXLOG(LDEBUG,"Test service cleanup.");
  run_discovery_service_test(test_service_cleanup);

  JNXLOG(LDEBUG, "Test starting discovery service.");
  run_discovery_service_test(test_starting_service);

  JNXLOG(LDEBUG, "Test stopping discovery service.");
  run_discovery_service_test(test_stopping_service);

  JNXLOG(LDEBUG, "Test restarting discovery service.");
  run_discovery_service_test(test_restarting_service);

  JNXLOG(LDEBUG, "Test setting peer_update_interval global variable.");
  run_discovery_service_test(test_setting_peer_update_interval);

  JNXLOG(LDEBUG, "Test broadcast update strategy.");
  run_discovery_service_test(test_broadcast_update_strategy);

  JNXLOG(LDEBUG, "Test polling update strategy.");
  run_discovery_service_test(test_polling_update_strategy);
  
  free(baddr);
  JNXLOG_DESTROY();
  return 0;
}
