/*
 * =====================================================================================
 *
 *       Filename:  peerstore.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/01/2015 08:07:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __PEERSTORE_H__
#define __PEERSTORE_H__

#include <time.h>
#include <jnxc_headers/jnxthread.h>
#include "peer.h"

typedef int (*is_active_peer_t)(time_t last_update_time, peer *p);

typedef struct {
  peer *local_peer;
  jnx_thread_mutex *store_lock;
  time_t last_update;
  is_active_peer_t is_active_peer;
  void *namestore;
  void *peers;
} peerstore;

peerstore *peerstore_init(peer *local_peer, is_active_peer_t is_active_peer);
peer *peerstore_get_local_peer(peerstore *ps);
void peerstore_store_peer(peerstore *ps, peer *p);
void peerstore_destroy(peerstore **ps);
peer *peerstore_lookup(peerstore *ps, jnx_guid *guid);
peer *peerstore_lookup_by_username(peerstore *ps, char *username);
int peerstore_get_active_guids(peerstore *ps, jnx_guid ***guids);
void peerstore_set_last_update_time(peerstore *ps, time_t last_update);

#endif
