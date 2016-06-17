#ifndef __CONNECTION_CONTROLLER_H__
#define __CONNECTION_CONTROLLER_H__
#include "peer.h"
#include "discovery.h"
#include "connection_request.h"
#include <jnxc_headers/jnx_list.h>
#include <whisper_protocol_headers/wpmux.h>
#include <whisper_protocol_headers/wpprotocol.h>

typedef enum connection_controller_state {
	E_CCS_OKAY,
	E_CCS_FAILED
}connection_controller_state;

/* Notification of connection completed */
typedef void (*connection_controller_notification_connection_completed)
  (const connection_request *c);
/* Notification of new connection incoming */
typedef void (*connection_controller_notification_connection_incoming)
  (const connection_request *c);
/* Notification of connection has been closed */
typedef void (*connection_controller_notification_connection_closed)(
    const connection_request *c);


typedef struct connection_controller {
	wp_mux *mux;
	jnx_char *port;
	const discovery_service *ds;
	jnx_list *connections;
        connection_controller_notification_connection_completed nc;
        connection_controller_notification_connection_incoming ic;
        connection_controller_notification_connection_closed cc;
}connection_controller;


connection_controller *connection_controller_create(jnx_char *traffic_port, 
    jnx_uint8 family, 
	const discovery_service *ds,
        connection_controller_notification_connection_completed nc,
        connection_controller_notification_connection_incoming ic,
        connection_controller_notification_connection_closed cc);

void *connection_controller_destroy(connection_controller **controller);

void connection_controller_tick(connection_controller *controller);

connection_controller_state connection_controller_initiation_request(
    connection_controller *controller, 
	peer *remote, connection_request **outrequest);

connection_request_state connection_controller_fetch_state(connection_request 
    *request);

connection_controller_state connection_controller_add_connection_request(
    connection_controller *controller,
	connection_request *c);

connection_controller_state connection_controller_remove_connection_request(
    connection_controller *controller,
	connection_request *c);
#endif
