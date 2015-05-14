/*
 * =====================================================================================
 *
 *       Filename:  test_serialisation.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  28/01/2015 15:42:24
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
#include <jnxc_headers/jnxcheck.h>
#include <jnxc_headers/jnxlog.h>
#include "peer.h"

void test_peer_serialisation() {
	jnx_guid gd;
	int i;
	for (i = 0; i < 16; i++) {
		gd.guid[i] = (uint8_t) 0x01;
	}	

	peer *p = peer_create(gd, "127.0.0.1", "0123456789PublicKey", 10);
	uint8_t *buffer;
	size_t length = peerton(p, &buffer);

	uint8_t expected[] = { 
		0x0a, 0x10, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
		0x01, 0x01, 0x12, 0x09, 0x31, 0x32, 0x37, 0x2e, 
		0x30, 0x2e, 0x30, 0x2e, 0x31, 0x1a, 0x13, 0x30, 
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 
		0x39, 0x50, 0x75, 0x62, 0x6c, 0x69, 0x63, 0x4b, 
		0x65, 0x79, 0x20, 0x0a };
	JNXCHECK(length == sizeof(expected));
	for (i = 0; i < length; i++) {
		JNXCHECK(buffer[i] == expected[i]);
	}

	free(buffer);
	peer_free(&p);
	JNXCHECK(NULL == p);
}
void test_peer_deserialisation() {
	int input_len = 52;
	uint8_t input[] = { 
		0x0a, 0x10, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
		0x01, 0x01, 0x12, 0x09, 0x31, 0x32, 0x37, 0x2e, 
		0x30, 0x2e, 0x30, 0x2e, 0x31, 0x1a, 0x13, 0x30, 
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 
		0x39, 0x50, 0x75, 0x62, 0x6c, 0x69, 0x63, 0x4b, 
		0x65, 0x79, 0x20, 0x0a };
	peer *p = ntopeer(input, input_len);

	int i;
	for (i = 0; i < 16; i++) {
		JNXCHECK(p->guid.guid[i] == 0x01);
	}
	JNXCHECK(0 == strcmp(p->host_address, "127.0.0.1"));
	JNXCHECK(0 == strcmp(p->user_name, "0123456789PublicKey"));
  JNXCHECK(10 == p->discovery_interval);
	peer_free(&p);
	JNXCHECK(NULL == p);
}
int main(int argc, char **argv) {
	JNXLOG(0, "Test peer to network serialisation.");
	test_peer_serialisation();

	JNXLOG(0, "Test network to peer deserialisation.");
	test_peer_deserialisation();

	return 0;
}
