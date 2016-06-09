#ifndef __CONNECTION_H__
#define __CONNECTION_H__

typedef enum connection_request_state {
	E_CRS_FAILED,
	E_CRS_UNKNOWN,
	E_CRS_PREHANDSHAKE,
	E_CRS_HANDSHAKECOMPLETE

}connection_request_state;

typedef struct connection_request {
	connection_request_state state;

}connection_request;

#endif