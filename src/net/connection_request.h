#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include <jnxc_headers/jnx_guid.h>

typedef enum connection_request_state {
	E_CRS_FAILED,
	E_CRS_UNKNOWN,
	E_CRS_PREHANDSHAKE,
	E_CRS_HANDSHAKECOMPLETE,
	E_CRS_CREATED

}connection_request_state;

typedef struct connection_request {
	jnx_guid id;
	connection_request_state state;

}connection_request;


connection_request *connection_request_create();

void connection_request_update_state(connection_request *req, connection_request_state s);

void connection_request_destroy(connection_request **req);

#endif
