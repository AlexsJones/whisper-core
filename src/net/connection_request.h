#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include <jnxc_headers/jnx_guid.h>
#include "peer.h"
#include "cryptography.h"
#include "discovery.h"
#include <whisper_protocol_headers/wpprotocol.h>

typedef enum connection_request_state {
	E_CRS_FAILED,
	E_CRS_UNKNOWN,
	E_CRS_INITIAL_CHALLENGE,
	E_CRS_CHALLENGE_REPLY,
	E_CRS_SESSION_KEY_SHARE,
	E_CRS_COMPLETE,
	E_CRS_CREATED

}connection_request_state;

typedef struct connection_request {
	jnx_guid id;
	connection_request_state state;
    peer *local;
    peer *remote;
    const discovery_service *ds;
    RSA *keypair;
}connection_request;

connection_request *connection_request_create(peer *local, peer *remote,const discovery_service *ds);

Wpmessage *connection_request_create_initiation_message(connection_request *req,connection_request_state s);

Wpmessage *connection_request_create_exchange_message(connection_request *req, Wpmessage *incoming_message, 
	connection_request_state s);

void connection_request_update_state(connection_request *req, connection_request_state s);

void connection_request_destroy(connection_request **req);

#endif
