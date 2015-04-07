/*
 * =====================================================================================
 *
 *       Filename:  session_service.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2015 05:50:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SESSION_SERVICE_H__
#define __SESSION_SERVICE_H__
#include <jnxc_headers/jnxguid.h>
#include <jnxc_headers/jnxlist.h>
#include <jnxc_headers/jnxcheck.h>
#include "session.h"
#include "handshake_control.h"
#include "peer.h"

typedef struct session_service {
  jnx_list *session_list;
}session_service;

session_service *session_service_create();

void session_service_destroy(session_service **service);

session_state session_service_create_shared_session(session_service *service,\
    jnx_char *input_guid_string,session **osession);

session_state session_service_create_session(session_service *service,session \
    **osession);

session_state session_service_fetch_session(session_service *service,jnx_guid \
    *session_guid, session **osession);

session_state session_service_fetch_all_sessions(session_service *service,\
    jnx_list **olist);

session_state session_service_destroy_session(session_service *service,jnx_guid \
    *session_guid);

session_state session_service_link_sessions(session_service *s,jnx_guid \
    *session_guid, peer *local_peer, peer *remote_peer);

session_state session_service_unlink_sessions(session_service *s,jnx_guid \
    *session_guid);

jnx_int session_service_session_is_linked(session_service *,jnx_guid \
    *session_guid);

#endif
