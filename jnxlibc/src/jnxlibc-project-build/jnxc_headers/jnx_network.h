/*
 * =====================================================================================
 *
 *       Filename:  jnxnetwork.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/02/14 17:09:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __JNXNETWORK_H__
#define __JNXNETWORK_H__
#include "jnx_types.h"
#include <ifaddrs.h>
#include <inttypes.h>
#ifdef __cplusplus
extern "C" {
#endif
#define AF_INET4_LOCALHOST "127.0.0.1"
#define AF_INET6_LOCALHOST "::1"

  typedef enum JNX_HTTP_TYPE {
    JNX_HTTP_POST,
    JNX_HTTP_GET

  } JNX_HTTP_TYPE;
  typedef enum JNX_HTTP_STATE {
    JNX_HTTP_STATE_OKAY,
    JNX_HTTP_STATE_FAIL,
    JNX_HTTP_STATE_UNKNOWN
  } JNX_HTTP_STATE;
  typedef enum JNX_NETWORK_ENDIAN {
    JNX_LITTLE_ENDIAN,
    JNX_BIG_ENDIAN,
    JNX_UNKNOWN_ENDIAN
  }JNX_NETWORK_ENDIAN;
  /*
   * @fn jnx_network_get_endianness
   * @brief an enum that indicates the endien precedence on the system
   * @return an enum value of JNX_NETWORK_ENDIAN
   */
  JNX_NETWORK_ENDIAN jnx_network_get_endianness();

  /*
   * @fn int jnx_network_interface_ip(jnx_char **obuffer,jnx_char *interface, jnx_unsigned_int family)
   * @brief This will attempt to convert the chosen interface and family into a string representing the IP address
   * @param obuffer is pointer to the out buffer that will contained stored data
   * @param interface is the name of the interface to use (accepts const jnx_char *), will accept NULL but will return the default interface
   * @param family is the family type to use - either AF_INET or AF_INET6
   * @return 0 on success, return 1 on error, return 2 on default interface used 
   */
  jnx_int32 jnx_network_interface_ip(jnx_char **obuffer,jnx_char *interface, jnx_unsigned_int family);
  /*
   * @fn int jnx_network_hostname_to_ip(unsigned hint_family,jnx_char *host, jnx_char **out_ip, jnx_unsigned_int *out_addrfamily);
   * @brief This will attempt to use your hint for AF_INET|IAF_INET6 to resolve the ip address from hostname e.g. google.com
   * @param hint_family must be either AF_INET or AF_INET6
   * @param host is the ip string to use
   * @param out_ip is a pointer to string pointer you wish to have populated
   * @param out_addrfamily is a pointer to a pointer you wish to store the found address family
   * @return 0 on success
   */
  jnx_int32 jnx_network_hostname_to_ip(unsigned hint_family,jnx_char *host, jnx_char **out_ip,jnx_unsigned_int *out_addrfamily);
  /* 
   * @fn size_t jnx_http_request(JNX_HTTP_TYPE type, const jnx_char *hostname, const jnx_char *page, jnx_char *args, jnx_uint8 **out_reply, jnx_size *out_len)
   * @brief creates an http request defined within the JNX_HTTP_TYPE (JNX_HTTP_GET or JNX_HTTP_POST)
   * @param type will specify the type of http request. Either JNX_HTTP_GET or JNX_HTTP_POST
   * @param hostname is the host domain to connect to.
   * @param page is the hostname page to display.
   * @params args are the optional arguments to add to the outward message
   * @params out_reply is a pointer to the pointer of jnx_uint8 array that contains the outward reply
   * @params out_len is the length of outward reply
   * @return message length sent in bytes 
   */
  size_t jnx_http_request(JNX_HTTP_TYPE type, const jnx_char *hostname, 
      const jnx_char *page, jnx_char *args, jnx_uint8 **out_reply,jnx_size *out_len);

  JNX_HTTP_TYPE jnx_http_request_post(const jnx_char *hostname, 
      const jnx_char *page, jnx_char *args,jnx_uint8 **out_reply, jnx_size *out_len);

  JNX_HTTP_TYPE jnx_http_request_get(const jnx_char *hostname,
      const jnx_char *page, jnx_char *args,jnx_uint8 **out_reply, jnx_size *out_len);

#ifdef __cplusplus
}
#endif
#endif
