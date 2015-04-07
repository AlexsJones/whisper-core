/*
 * =====================================================================================
 *
 *       Filename:  peerstore.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/01/2015 08:07:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <jnxc_headers/jnxhash.h>
#include <jnxc_headers/jnxguid.h>
#include "peerstore.h"

#define PEERSTORE(x) ((jnx_hashmap *) x)
#define NAMESTORE(x) ((jnx_hashmap *) x)

static int is_peer_active(peerstore *ps, peer *p) {
  return ps->is_active_peer(ps->last_update, p);
}
static int always_active(time_t lut, peer *p) {
  return 1;
}
peerstore *peerstore_init(peer *local_peer, is_active_peer_t iap) {
  peerstore *store = malloc(sizeof(peerstore));
  store->local_peer = local_peer;
  store->store_lock = jnx_thread_mutex_create();
  store->peers = (void *) jnx_hash_create(1024);
  store->namestore = (void *) jnx_hash_create(1024);
  store->last_update = time(0);
  if (iap == NULL) {
    store->is_active_peer = always_active;
  }
  else {
    store->is_active_peer = iap;
  }
  return (peerstore *) store;
}
peer *peerstore_get_local_peer(peerstore *ps) {
  return ps->local_peer;
}
static void handle_peer_reconnection(peerstore *ps, peer *p) {
  char *guid_str = (char *) jnx_hash_get(NAMESTORE(ps->namestore), p->user_name);
  if (guid_str != NULL) {
    char *new_guid_str;
    jnx_guid_to_string(&p->guid, &new_guid_str);
    if (0 != strcmp(guid_str, new_guid_str)) {
      peer *old_peer = jnx_hash_get(PEERSTORE(ps->peers), guid_str);
      if (old_peer != NULL
          && 0 == strcmp(p->host_address, old_peer->host_address)) {
        jnx_hash_delete_value(PEERSTORE(ps->peers), guid_str);
        peer_free(&old_peer);
        free(guid_str);
      }
    }
  }
}
void peerstore_store_peer(peerstore *ps, peer *p) {
  jnx_thread_lock(ps->store_lock);
  handle_peer_reconnection(ps, p);
  p->last_seen = time(0);
  char *guid_str;
  jnx_guid_to_string(&p->guid, &guid_str);
  peer *old = jnx_hash_get(PEERSTORE(ps->peers), guid_str);
  if (old != NULL) {
    memcpy(&old->guid, &p->guid, sizeof(jnx_guid));
   
    free(old->host_address);
    old->host_address = calloc(1 + strlen(p->host_address), sizeof(char));
    strcpy(old->host_address, p->host_address);
   
    free(old->user_name);
    old->user_name = calloc(1 + strlen(p->user_name), sizeof(char));
    strcpy(old->user_name, p->user_name);

    old->last_seen = p->last_seen;

    peer_free(&p);
  }
  else {
    jnx_hash_put(PEERSTORE(ps->peers), guid_str, (void *) p);
    jnx_hash_put(NAMESTORE(ps->namestore), p->user_name, (void *) guid_str);
  }
  jnx_thread_unlock(ps->store_lock);
}
void peerstore_destroy(peerstore **pps) {
  peerstore *ps = *pps;
  peer_free(&(ps->local_peer));

  jnx_hashmap *peers = PEERSTORE(ps->peers);
  jnx_hashmap *namestore = NAMESTORE(ps->namestore);
  const char **keys;
  int num_keys = jnx_hash_get_keys(peers, &keys); 
  int i;
  for (i = 0; i < num_keys; i++) {
    peer *temp = jnx_hash_get(peers, *(keys + i));
    peer_free(&temp);
  }
  jnx_hash_destroy(&peers);
  jnx_hash_destroy(&namestore);
  jnx_thread_mutex_destroy(&ps->store_lock);
  free(ps);
  *pps = NULL;
}
peer *peerstore_lookup(peerstore *ps, jnx_guid *guid) {
  JNXCHECK(ps->is_active_peer);
  jnx_thread_lock(ps->store_lock);
  char *guid_str;
  jnx_guid_to_string(guid, &guid_str);
  peer *p = (peer *) jnx_hash_get(PEERSTORE(ps->peers), guid_str);
  if (p != NULL && !is_peer_active(ps, p)) {
    jnx_hash_delete_value(PEERSTORE(ps->peers), guid_str);
    peer_free(&p);
    p = NULL;
  }
  free(guid_str);
  jnx_thread_unlock(ps->store_lock);
  return p;
}
peer *peerstore_lookup_by_username(peerstore *ps, char *username) {
  JNXCHECK(ps->is_active_peer);
  jnx_thread_lock(ps->store_lock);
  char *guid_str = (char *) jnx_hash_get(NAMESTORE(ps->namestore), username);
  if (guid_str == NULL) {
    jnx_thread_unlock(ps->store_lock);
    return NULL;
  }
  peer *p = (peer *) jnx_hash_get(PEERSTORE(ps->peers), guid_str);
  if (p == NULL) {
    jnx_hash_delete_value(NAMESTORE(ps->namestore), username);
    free(guid_str);
    jnx_thread_unlock(ps->store_lock);
    return NULL;
  }
  if (p != NULL && !is_peer_active(ps, p)) {
    jnx_hash_delete_value(PEERSTORE(ps->peers), guid_str);
    peer_free(&p);
    p = NULL;
  }
  jnx_thread_unlock(ps->store_lock);
  return p;
}
void peerstore_peer_no_longer_active(peerstore *ps, peer *p) {
  jnx_thread_lock(ps->store_lock);
  char *guid_str = jnx_hash_get(NAMESTORE(ps->namestore), p->user_name);
  jnx_hash_delete_value(NAMESTORE(ps->namestore), p->user_name);
  peer *pr = (peer *) jnx_hash_get(PEERSTORE(ps->peers), guid_str);
  jnx_hash_delete_value(PEERSTORE(ps->peers), guid_str);
  peer_free(&pr);
  free(guid_str);
  jnx_thread_unlock(ps->store_lock);
}
int peerstore_get_active_guids(peerstore *ps, jnx_guid ***guids) {
  jnx_thread_lock(ps->store_lock);
  jnx_hashmap *peers = PEERSTORE(ps->peers);
  const char **keys;
  int num_keys = jnx_hash_get_keys(peers, &keys); 
  int i, num_guids = 0;
  *guids = calloc(num_keys, sizeof(jnx_guid *));
  for (i = 0; i < num_keys; i++) {
    peer *temp = jnx_hash_get(peers, *(keys + i));
    if (is_peer_active(ps, temp)) {
      (*guids)[num_guids] = &temp->guid;
      num_guids++;
    }	
  }
  jnx_thread_unlock(ps->store_lock);
  return num_guids;
}
void peerstore_set_last_update_time(peerstore *ps, time_t last_update) {
  jnx_thread_lock(ps->store_lock);
  ps->last_update = last_update;
  jnx_thread_unlock(ps->store_lock);
}
