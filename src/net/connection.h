#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include "peer.h"
#include "discovery.h"
#include <whisper_protocol_headers/wpmux.h>
#include <whisper_protocol_headers/wpprotocol.h>

typedef enum connection_request_state {
	E_CRS_UNKNOWN,
	E_CRS_PREHANDSHAKE,
	E_CRS_HANDSHAKECOMPLETE

}connection_request_state;

typedef struct connection_request {
	connection_request_state state;

}connection_request;


typedef struct connection_controller {
	wp_mux *mux;
	jnx_char *port;
	const discovery_service *ds;

}connection_controller;


connection_controller *connection_controller_create(jnx_char *traffic_port, jnx_uint8 family, 
	const discovery_service *ds);

void *connection_controller_destroy(connection_controller **controller);

void connection_controller_tick(connection_controller *controller);

const connection_request* connection_controller_initiation_request(connection_controller *controller, 
	peer *local, peer *remote);

connection_request_state connection_controller_fetch_state(connection_request *request);


#endif