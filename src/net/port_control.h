#ifndef __PORT_CONTROL_H__
#define __PORT_CONTROL_H__
#include <jnxc_headers/jnxtypes.h>
#include <jnxc_headers/jnxcheck.h>
typedef struct port_control {
	jnx_int lrange;
	jnx_int urange;
	jnx_int interval;

}port_control;

typedef enum E_SCAN_STATE {
  FULL_SCAN,
  RETURN_EARLY_SCAN
}E_SCAN_STATE;
jnx_int port_control_scan_range(port_control *p,E_SCAN_STATE state);

jnx_int port_control_next_available(port_control *p);

jnx_char* port_control_next_available_to_s(port_control *p);

port_control *port_control_create(jnx_int lrange, 
    jnx_int urange, jnx_int interval);

void port_control_destroy(port_control **p);

#endif
