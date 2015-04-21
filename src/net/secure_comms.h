/*
 * secure_comms.h
 * Copyright (C) 2015 tibbar <tibbar@ubuntu>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef SECURE_COMMS_H
#define SECURE_COMMS_H
#include "session.h"
#include "discovery.h"
#include <jnxc_headers/jnxtypes.h>
/* Called from both remote endpoints near simultaneously */
typedef enum {
  SC_INITIATOR,
  SC_RECEIVER
}secure_comms_endpoint;

/*
 *@fn jnx_int secure_comms_receiver_start(discovery_service *ds,
     session *s,jnx_unsigned_int addr_family)
 *@brief This function binds a secure_comms_file descriptor for usage
 @param ds is the discovery service reference
 @param s is the current session to attach secure_comms_fd too
 @param addr_family is the family (AFINET or AFINET6)
 @return bound socket file descriptor (also available on s->secure_comms_fd)
*/
jnx_int secure_comms_receiver_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family);
/*
  *@fn jnx_int secure_comms_receiver_start(discovery_service *ds,
  session *s,jnx_unsigned_int addr_family)
  *@brief This function binds a secure_comms_file descriptor for usage
  @param ds is the discovery service reference
  @param s is the current session to attach secure_comms_fd too
  @param addr_family is the family (AFINET or AFINET6)
  @return bound socket file descriptor (also available on s->secure_comms_fd)
*/
jnx_int secure_comms_initiator_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family);

void secure_comms_end(session *s);

#endif /* !SECURE_COMMS_H */
