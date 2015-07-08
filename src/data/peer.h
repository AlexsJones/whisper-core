/*
 * =====================================================================================
 *
 *       Filename:  peer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/01/2015 15:49:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  The Achievers
 *
 * =====================================================================================
 */

#ifndef __PEER_H__
#define __PEER_H__

#include <time.h>
#include <jnxc_headers/jnxguid.h>

typedef enum {
  PEERS_EQUIVALENT,
  PEERS_DIFFERENT
} peer_compare_status;

typedef struct {
  jnx_guid guid;
  char *host_address;
  char *user_name;
  jnx_uint32 discovery_interval;
  time_t last_seen;
} peer;

size_t peerton(peer *p, void **out);

peer *ntopeer(void *in, size_t len);

void peer_free(peer **p);

peer *peer_create(jnx_guid guid, char *host_address, char *user_name, jnx_uint32 discovery_interval);

peer *local_peer_for_user(char *username, jnx_uint32 discovery_interval);

peer_compare_status peers_compare(peer *p1, peer *p2);

#endif
