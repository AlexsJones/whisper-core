#include "connection_request.h"

connection_request *connection_request_create() {
	connection_request *r = malloc(sizeof(connection_request));
	jnx_guid_create(&(*r).id);
}
void connection_request_update_state(connection_request *req, connection_request_state s) {
	req->state = s;
}
void connection_request_destroy(connection_request **req) {
	JNXCHECK(*req);
	free(*req);
}