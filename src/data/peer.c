/*
 * =====================================================================================
 *
 *       Filename:  peer.c
AirlineTheme badwolf
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/01/2015 15:51:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include <jnxc_headers/jnxlog.h>
#include "peer.h"
#include "peer.pb-c.h"
#include "../net/discovery.h"

size_t peerton(peer *p, void **out) {
  Peer msg = PEER__INIT;
  size_t len;

  // required bytes guid=1;
  msg.guid.len = 16;
  msg.guid.data = malloc(sizeof(char) * msg.guid.len);
  int i;
  for (i = 0; i < msg.guid.len; i++) {
    msg.guid.data[i] = p->guid.guid[i];
  }

  // required string host_address=2;
  msg.host_address = calloc(1 + strlen(p->host_address), sizeof(char));
  strcpy(msg.host_address, p->host_address);

  // optional string user_name=3;
  if (0 != p->user_name) {
    msg.user_name = malloc(1 + sizeof(char) * strlen(p->user_name));
    strcpy(msg.user_name, p->user_name);
  }

  msg.discovery_interval = p->discovery_interval;

  len = peer__get_packed_size(&msg);
  *out = malloc(len);
  peer__pack(&msg, *out);

  return len;
}
peer *ntopeer(void *in, size_t len) {
  Peer *msg;
  msg = peer__unpack(NULL, len, in);
  if (NULL == msg) {
    JNX_LOG(NULL, "Error unpacking a peer message.");
    return NULL;
  }

  jnx_guid gd;
  memcpy(gd.guid, msg->guid.data, msg->guid.len);	
  peer *p = peer_create(gd, msg->host_address, msg->user_name, msg->discovery_interval); 
  peer__free_unpacked(msg, NULL);
  return p;	
}
peer *peer_create(jnx_guid guid, char *host_address, char *user_name, jnx_uint32 discovery_interval) {
  JNXCHECK(host_address);
  peer *temp = malloc(sizeof(peer));

  memcpy(temp->guid.guid, guid.guid, 16);
  temp->host_address = malloc(1 + strlen(host_address));
  strcpy(temp->host_address, host_address);
  if (NULL != user_name && strlen(user_name) > 0) {
    temp->user_name = malloc(1 + strlen(user_name));
    strcpy(temp->user_name, user_name);
  }
  temp->discovery_interval = discovery_interval;
  return temp;
}
void peer_free(peer **p) {
  peer *temp = *p;
  free(temp->host_address);
  if (NULL != temp->user_name) {
    free(temp->user_name);
  }
  free(temp);
  *p = NULL;
}
peer *local_peer_for_user(char *user, jnx_uint32 discovery_interval) {
	peer *p = calloc(1, sizeof(peer));
	jnx_guid_create(&p->guid);
	get_local_ip(&p->host_address);
	p->user_name = user;
  p->discovery_interval = discovery_interval;
	return p;
}
peer_compare_status peers_compare(peer *p1, peer *p2) {
  peer_compare_status status = PEERS_EQUIVALENT;
  status |= jnx_guid_compare(&p1->guid, &p2->guid);
  status |= strcmp(p1->host_address, p2->host_address);
  status |= strcmp(p1->user_name, p2->user_name);
  status &= 1;
  return status;
}
