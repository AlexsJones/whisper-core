/*
 * =====================================================================================
 *
 *       Filename:  discovery.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/01/15 14:57:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (draganglumac), dragan.glumac@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __DISCOVERY_H__
#define __DISCOVERY_H__

#include <jnxc_headers/jnx_types.h>
#include <jnxc_headers/jnx_udp_socket.h>
#include <jnxc_headers/jnx_thread.h>
#include <time.h>
#include "peer.h"
#include "peerstore.h"

typedef struct {
  int port;
  unsigned int family;
  jnx_socket *sock_send;
  jnx_udp_listener *udp_listener;
  char *broadcast_group_address;
  jnx_udp_listener_callback receive_callback;
  int isrunning;
  peerstore *peers; // synchronised
  jnx_thread *update_thread;
  jnx_thread *listening_thread;  
  time_t last_updated; // synchronised
  jnx_thread_mutex *update_time_lock;
} discovery_service;

typedef jnx_int32 (discovery_strategy)(discovery_service *);
extern discovery_strategy polling_update_strategy;
extern discovery_strategy broadcast_update_strategy;

extern int peer_update_interval; // seconds

#define INITIAL_DISCOVERY_REQS 3
#define ASK_ONCE_STRATEGY NULL
#define POLLING_UPDATE_STRATEGY polling_update_strategy
#define BROADCAST_UPDATE_STRATEGY broadcast_update_strategy

void get_local_ip(char **local_ip_buffer, char *interface);
void get_broadcast_ip(char **broadcast_ip_buffer, char *interface);

#define DEFAULT_BROADCAST_PORT 8704

discovery_service* discovery_service_create(int port, unsigned int family,
                                            char *broadcast_group_address, peerstore *peers);

void discovery_service_cleanup(discovery_service **svc);

int discovery_service_start(discovery_service *svc, discovery_strategy *strategy);

int discovery_service_stop(discovery_service *svc);

void discovery_notify_peers_of_shutdown(discovery_service *svc);

time_t get_last_update_time(discovery_service *svc);

void cancel_thread(jnx_thread **thr);
#endif
