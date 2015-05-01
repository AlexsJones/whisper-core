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
#include "port_control.h"
#include <jnxc_headers/jnxthread.h>

typedef int (*accept_reject_callback)(discovery_service *, 
    jnx_guid *, 
    jnx_guid *);

typedef struct auth_comms_service {
  jnx_tcp_listener *listener;
  jnx_thread *listener_thread;
  jnx_tcp_listener_callback listener_callback;
  accept_reject_callback ar_callback;
}auth_comms_service;

/*
 * @fn auth_comms_service *auth_comms_create()
 * @brief creates the auth_comms_service
 * @return a pointer to auth_comms_service struct
 */
auth_comms_service *auth_comms_create();
/*
 * @fn auth_comms_listener_start(auth_comms_service *ac,
    discovery_service *ds,session_service *ss,void *linking_args)
 * @brief starts a listener for auth_comms protobuf objects
 * @param ac is a pointer to initialised auth_comm_service
 * @param dc is a pointer to initialised discovery_service
 * @param ss is a pointer to initialised session_service
 * @param linking_args are any arguments that should be returned to user space during the
 * receiver linking process
 */
void auth_comms_listener_start(auth_comms_service *ac,
    discovery_service *ds,session_service *ss,void *linking_args);
/*
 * @fn auth_comms_destroy(auth_comms_service **ac)
 * @brief destroys the auth_comms_service
 * @param ac is a double pointer to auth_comms_service
 */
void auth_comms_destroy(auth_comms_service **ac);
/*
 * @fn auth_comms_initiator_start(auth_comms_service *ac, \
 *   discovery_service *ds, port_control_service *ps,
 *   session *s)
 *   @brief starts the handshaking process between two peers
 *   that eventually leads to secure comms and returns valid sock fd's
 *   with full encryption to the session
 * @param ac is a pointer to initialised auth_comm_service
 * @param dc is a pointer to initialised discovery_service
 * @param ps is a pointer to initialised port_control_service
 * @param s is a pointer to the current session
 */
void auth_comms_initiator_start(auth_comms_service *ac, \
    discovery_service *ds, port_control_service *ps, 
    session *s);
/*
 * @fn auth_comms_stop(auth_comms_service *ac,session *s)
 * @brief stops the current auth_comms session by closing sockets
 * and releasing data
 * @param ac is a pointer to initialised auth_comm_service
 * @param s is a pointer to the current session
 */
void auth_comms_stop(auth_comms_service *ac,session *s);

#endif /* !AUTH_COMMS_H */
