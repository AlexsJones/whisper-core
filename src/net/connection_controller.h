#ifndef __CONNECTION_CONTROLLER_H__
#define __CONNECTION_CONTROLLER_H__
#include "peer.h"
#include "discovery.h"
#include "connection.h"
#include <jnxc_headers/jnx_list.h>
#include <whisper_protocol_headers/wpmux.h>
#include <whisper_protocol_headers/wpprotocol.h>

typedef struct connection_controller {
	wp_mux *mux;
	jnx_char *port;
	const discovery_service *ds;
	jnx_list *connections;
}connection_controller;


connection_controller *connection_controller_create(jnx_char *traffic_port, jnx_uint8 family, 
	const discovery_service *ds);

void *connection_controller_destroy(connection_controller **controller);

void connection_controller_tick(connection_controller *controller);

connection_request_state connection_controller_initiation_request(connection_controller *controller, 
	peer *local, peer *remote, connection_request **outrequest);

connection_request_state connection_controller_fetch_state(connection_request *request);


#endif