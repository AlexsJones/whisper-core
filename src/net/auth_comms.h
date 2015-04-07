/*
 * session_service_auth_comms.h
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef AUTH_COMMS_H
#define AUTH_COMMS_H
#include "discovery.h"
#include <jnxc_headers/jnx_tcp_socket.h>
#include "session.h"
#include "session_service.h"
#include <jnxc_headers/jnxthread.h>

typedef int (*accept_reject_callback)(discovery_service *, jnx_guid *, jnx_guid *);

typedef struct auth_comms_service {
  jnx_tcp_listener *listener;
  jnx_thread *listener_thread;
  jnx_tcp_listener_callback listener_callback;
  accept_reject_callback ar_callback;
}auth_comms_service;

auth_comms_service *auth_comms_create();

void auth_comms_listener_start(auth_comms_service *ac,discovery_service *ds,session_service *ss);

void auth_comms_destroy(auth_comms_service **ac);

void auth_comms_initiator_start(auth_comms_service *ac, \
    discovery_service *ds, session *s, jnx_char *port);

#endif /* !AUTH_COMMS_H */
