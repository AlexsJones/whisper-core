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

typedef enum linked_session_type {
  E_AM_RECEIVER,
  E_AM_INITIATOR
}linked_session_type;

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
}session_service;


/*
 @fn session_service *session_service_create(session_linking_service_func linking_func,
    session_unlinking_service_func unlinking_func)
 @brief Creates the session service that controls all session creation and linking
 @param linking_func is the chance for the user to provide a functor for any additional operations
 during the linking time e.g. spawn threads, save data etc
 must return 0 on success!
 @param unlinking_func is much the same as the linking func and gives a first chance before unlinking
 must return 0 on success!
 @return session_service pointer
*/
session_service *session_service_create(session_linking_service_func linking_func,
    session_unlinking_service_func unlinking_func);
/*
 @fn session_service_destroy(session_service **service)
 @brief destroys the session service and all of its sessions
 @param service is a double pointer to the service and will set service to NULL
*/
void session_service_destroy(session_service **service);
/*
 @fn session_state session_service_create_shared_session(session_service *service,\
    jnx_char *input_guid_string,session **osession)
 @brief this creates a session by way of existing input guid, for use in replication over network
 @param service pointer to session service
 @param input_guid_string is the existing session guid to replicate
 @param osession is the double pointer for outward session 
 @return session_state
*/
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

jnx_int session_service_session_is_linked(session_service *,jnx_guid \
    *session_guid);

#endif
