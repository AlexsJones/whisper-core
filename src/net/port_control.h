#ifndef __PORT_CONTROL_H__
#define __PORT_CONTROL_H__
#include <jnxc_headers/jnxtypes.h>
#include <jnxc_headers/jnxcheck.h>
typedef struct port_control_service {
	jnx_int lrange;
	jnx_int urange;
	jnx_int interval;

}port_control_service;

typedef enum E_SCAN_STATE {
  FULL_SCAN,
  RETURN_EARLY_SCAN
}E_SCAN_STATE;
jnx_int port_control_service_scan_range(port_control_service *p,E_SCAN_STATE state);

jnx_int port_control_service_next_available(port_control_service *p);

jnx_char* port_control_service_next_available_to_s(port_control_service *p);

port_control_service *port_control_service_create(jnx_int lrange, 
    jnx_int urange, jnx_int interval);

void port_control_service_destroy(port_control_service **p);

#endif
