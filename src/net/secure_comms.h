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
/* Called from both remote endpoints simultaneously */
typedef enum {
  SC_INITIATOR,
  SC_RECEIVER
}secure_comms_endpoint;

void secure_comms_start(secure_comms_endpoint e, discovery_service *ds,
    session *s,jnx_unsigned_int addr_family); 

void secure_comms_receiver_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family);

void secure_comms_initiator_start(discovery_service *ds,
    session *s,jnx_unsigned_int addr_family); 
#endif /* !SECURE_COMMS_H */
