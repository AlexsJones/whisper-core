/*
 * =====================================================================================
 *
 *       Filename:  test_peerstore.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  24/02/2015 16:35:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <jnxc_headers/jnxcheck.h>
#include <jnxc_headers/jnxlog.h>
#include "peer.h"
#include "peerstore.h"

int inactive(time_t ts, peer *p) {
  return 0;
}
peer *local_test_peer() {
  jnx_guid guid;
  int i;
  for (i = 0; i < 16; i++) {
    guid.guid[i] = i;
  }
  return peer_create(guid, "127.0.0.1", "Local User", 10);
}
peer *other_test_peer() {
  jnx_guid guid;
  int i;
  for (i = 0; i < 16; i++) {
    guid.guid[i] = 15;
  }
  return peer_create(guid, "127.0.0.2", "Another", 10);
}
peer *bob1() {
  jnx_guid guid;
  int i;
  for (i = 0; i < 16; i++) {
    guid.guid[i] = i + 1;
  }
  return peer_create(guid, "127.0.0.3", "Bob", 10);
}
peer *bob2() {
  jnx_guid guid;
  int i;
  for (i = 0; i < 16; i++) {
    guid.guid[i] = i + 2;
  }
  return peer_create(guid, "127.0.0.4", "Bob", 10);
}
peerstore *create_test_peerstore() {
  peerstore *store = peerstore_init(local_test_peer(), 0);
  return store;
}
void test_peerstore_lookup_by_username() {
  peerstore *ps = create_test_peerstore();
  peerstore_store_peer(ps, local_test_peer());
  peerstore_store_peer(ps, other_test_peer());

  peer *other = peerstore_lookup_by_username(ps, "Another");
  peer *local = peerstore_lookup_by_username(ps, "Local User");
  
  int status = peers_compare(other, peerstore_get_local_peer(ps));
  JNXCHECK(status == PEERS_DIFFERENT);
  status = peers_compare(local, peerstore_get_local_peer(ps));
  JNXCHECK(status == PEERS_EQUIVALENT);
  peerstore_destroy(&ps);
}
void test_peerstore_username_clash() {
  peerstore *ps = create_test_peerstore();
  peer *b1 = bob1();
  peer *b2 = bob2();

  peerstore_store_peer(ps, b1);
  peerstore_store_peer(ps, b2);

  int status = peers_compare(b1, peerstore_lookup_by_username(ps, "Bob"));
  JNXCHECK(status == PEERS_EQUIVALENT);
  status = peers_compare(b2, peerstore_lookup_by_username(ps, "Bob-0203"));
  JNXCHECK(status == PEERS_EQUIVALENT);
  peerstore_destroy(&ps);
}
void test_peerstore_lookup_for_non_existant_username() {
  peerstore *ps = create_test_peerstore();
  peer *missing = peerstore_lookup_by_username(ps, "MissingUser");
  JNXCHECK(NULL == missing);
  peerstore_destroy(&ps);
}
void test_peerstore_lookup_for_inactive_username() {
  peerstore *ps = create_test_peerstore();
  peerstore_store_peer(ps, other_test_peer());
  ps->is_active_peer = inactive;
  peer *other = peerstore_lookup_by_username(ps, "OtherUser");
  JNXCHECK(NULL == other);
  peerstore_destroy(&ps);
}
int main() {
  JNXLOG(0, "test_peerstore_lookup_by_username");
  test_peerstore_lookup_by_username();

  JNX_LOG(0, "test_peerstore_username_clash");
  test_peerstore_username_clash();

  JNXLOG(0, "test_peerstore_lookup_for_non_existant_username");
  test_peerstore_lookup_for_non_existant_username();
  
  JNXLOG(0, "test_peerstore_lookup_for_inactive_username");
  test_peerstore_lookup_for_inactive_username();

  return 0;
}
