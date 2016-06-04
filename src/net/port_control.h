#ifndef __PORT_CONTROL_H__
#define __PORT_CONTROL_H__
#include <jnxc_headers/jnx_types.h>
#include <jnxc_headers/jnx_check.h>
typedef struct port_control_service {
	jnx_int lrange;
	jnx_int urange;
	jnx_int crange;
        jnx_int interval;
}port_control_service;

typedef enum E_SCAN_STATE {
  FULL_SCAN,
  RETURN_EARLY_SCAN
}E_SCAN_STATE;
/*
 * @fn jnx_int port_control_service_scan_range(port_control_service *p,E_SCAN_STATE state)
 * @brief this function is primarily for internal use but can be set to return
 * either early or run a full scan of ports and return the last
 * @param p is a pointer to the port_control_service struct
 * @param E_SCAN_STATE is an enum that can either be set to FULL_SCAN or RETURN_EARLY_SCAN
 * @return -1 on error, else port that is open
 */
jnx_int port_control_service_scan_range(port_control_service *p,E_SCAN_STATE state);
/*
 * @fn jnx_int port_control_service_next_available(port_control_service *p)
 * @brief returns the next available port in the stored ranges and interval
 * @param p is a pointer to the port_control_service struct
 * @return -1 on error, else port that is open
 */
jnx_int port_control_service_next_available(port_control_service *p);
/*
 * @fn jnx_char* port_control_service_next_available_to_s(port_control_service *p)
 * @brief returns from port_control_service_next_available and converts port into a jnx_char*
 * @param p is a pointer to the port_control_service struct
 * @return -1 on error, else port that is open
 */
jnx_char* port_control_service_next_available_to_s(port_control_service *p);
/*
 * @fn port_control_service *port_control_service_create(jnx_int lrange,
    jnx_int urange, jnx_int interval)
 * @brief creates the port control service with a range of values
 * @param lrange is the lowest port to search for
 * @param urange is the upper port range to search for
 * @param interval is the gap between ports to check
 * @returns pointer to port_control_service
 */
port_control_service *port_control_service_create(jnx_int lrange, 
    jnx_int urange, jnx_int interval);
/*
 * @fn port_control_service_destroy(port_control_service **p)
 * @brief destroys the port_control_service structure
 * @param a double pointer to port control service
 */
void port_control_service_destroy(port_control_service **p);

#endif
