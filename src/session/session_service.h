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
#include <jnxc_headers/jnx_guid.h>
#include <jnxc_headers/jnx_list.h>
#include <jnxc_headers/jnx_check.h>
#include <whisper_protocol_headers/wpmux.h>
#include <whisper_protocol_headers/wpprotocol.h>
#include "session.h"
#include "peer.h"

typedef enum linked_session_type {
  E_AM_RECEIVER,
  E_AM_INITIATOR
}linked_session_type;

typedef enum alert_session_update {
  E_ALERT_JOINED_FORIEGN_SESSION,
  E_ALERT_LEFT_FORIEGN_SESSION,
  E_ALERT_SESSION_CLOSED,
  E_ALERT_SESSION_ERROR
}alert_session_update;

typedef int (*session_linking_service_func)(session *s,
    linked_session_type session_type,
    void *optargs);

typedef int (*session_unlinking_service_func)(session *s,
    linked_session_type session_type,
    void *optargs);

typedef struct session_service {
  jnx_list *session_list;
  session_linking_service_func linking_func;
  session_unlinking_service_func unlinking_func;
  wp_mux *mux;
}session_service;

session_service *session_service_create(
    session_linking_service_func linking_func,
    session_unlinking_service_func unlinking_func,
    const wp_mux *mux);

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

session_state session_service_link_sessions(session_service *s,
    linked_session_type session_type,
    void *linking_args,
    jnx_guid *session_guid, peer *local_peer, peer *remote_peer);

session_state session_service_unlink_sessions(session_service *s,
    linked_session_type session_type,
    void *linking_args, jnx_guid \
    *session_guid);

jnx_int session_service_session_is_linked(session_service *service,jnx_guid \
    *session_guid);

void session_service_alert_session_update(session_service *service, session *s, 
    alert_session_update update);
#endif
