/*
 * jnx_udp_socket.h
 * Copyright (C) 2015 tibbar <tibbar@debian>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef JNX_UDP_SOCKET_H
#define JNX_UDP_SOCKET_H
#include "jnx_socket.h"
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct jnx_udp_listener {
    jnx_socket *socket;
    jnx_int hint_exit;
  }jnx_udp_listener;

  typedef void (*jnx_udp_listener_callback)(const jnx_uint8 *payload, \
      jnx_size bytes_read, void *args);
  
  /**
   * @fn jnx_socket *jnx_socket_udp_create(jnx_unsigned_int addrfamily)
   * @brief creates a jnx udp socket
   * @param addrfamily this is the address family to use (e.g. AF_INET)
   * @return jnx_socket
   */
  jnx_socket *jnx_socket_udp_create(jnx_unsigned_int addrfamily);
  /**
   *@fn jnx_udp_listener *jnx_socket_udp_listener_create(char *port,
   *jnx_unsigned_int family)
   *@brief Creates a listener that can be 
   *ticked and retrieve incoming network traffic
   *@param family is the address family to use (AF_INET/AF_INET6)
   *@return pointer to a listener on success
   */
  jnx_udp_listener* jnx_socket_udp_listener_create(jnx_char *port,
      jnx_unsigned_int family);
    /**
   *@fn jnx_udp_listener *jnx_socket_udp_listener_broadcast_create(char *port,
   *jnx_unsigned_int family)
   *@brief Creates a listener that can be 
   *ticked and retrieve incoming network traffic whilst listening for broadcasts
   *@param family is the address family to use (AF_INET/AF_INET6)
   *@return pointer to a listener on success
   */
  jnx_udp_listener* jnx_socket_udp_listener_broadcast_create(jnx_char *port,
      jnx_unsigned_int family);
    /**
   *@fn jnx_udp_listener *jnx_socket_udp_listener_multicast_create(char *port,
   *jnx_unsigned_int family, int max_connections)
   *@brief Creates a listener that can be 
   *ticked and retrieve incoming network traffic and subscribes to the group
   *@param family is the address family to use (AF_INET/AF_INET6)
   *@param bgroup is the subscription group for multicasts
   *@return pointer to a listener on success
   */
  jnx_udp_listener* jnx_socket_udp_listener_multicast_create(jnx_char *port,
      jnx_unsigned_int family, jnx_char *ip, jnx_char *bgroup);
  /**
   *@fn void jnx_socket_udp_listener_destroy(jnx_udp_listener **listener)
   *@brief destroys and closes socket for the udp listener
   *@param listener is a pointer to pointer of the current listener
   */
  void jnx_socket_udp_listener_destroy(jnx_udp_listener **listener);
  /**
   *@fn jnx_socket_udp_listener_tick(jnx_udp_listener *listener,
   * jnx_udp_listener_callback, void *args)
   *@brief tick will need to be called manually by the user to accept and recv
   *incoming network data. It has been designed for use in a loop 
   *and to provide max user control
   *@warning NONBLOCKING
   *@param listener is the current instantiated listener
   *@param callback is passed into the tick representing the function
   *to return received data too
   *@param args are the context arguments to pass to the receiver function 
   *can be null
   *@returns 0 on success, -1 on failure
   */
  void jnx_socket_udp_listener_tick(jnx_udp_listener* listener,
      jnx_udp_listener_callback callback, void *args);
  /**
   *@fn jnx_socket_udp_listener_tick(jnx_udp_listener *listener,
   * jnx_udp_listener_callback, void *args)
   *@brief auto tick will block and loop automatically for the user
   *user to accept and recv incoming network data.
   *@warning BLOCKING
   *@param listener is the current instantiated listener
   *@param callback is passed into the tick representing the function and to cancel
   *the listener return -1 to exit or 0 to continue listening
   *to return received data too
   *@param args are the context arguments to pass to the receiver function 
   *can be null
   */
  void jnx_socket_udp_listener_auto_tick(jnx_udp_listener *listener,
      jnx_udp_listener_callback callback, void *args);
  /**
   * @fn jnx_char *jnx_socket_udp_resolve_ipaddress(struct sockaddr_storage sa);
   * @brief if successful it will return a string displaying 
   * the readable IP address
   * @param sa is the sockaddr_stoage to pass through from the accept call
   * @return jnx_char string or NULL
   */   
  jnx_char *jnx_socket_udp_resolve_ipaddress(struct sockaddr_storage sa);

  jnx_size jnx_socket_udp_send(jnx_socket *s, jnx_char *host,\
      jnx_char* port, jnx_uint8 *msg, jnx_size msg_len);
      
  jnx_size jnx_socket_udp_broadcast_send(jnx_socket *s, jnx_char *host,\
    jnx_char* port, jnx_uint8 *msg, jnx_size msg_len);

  jnx_size jnx_socket_udp_multicast_send(jnx_socket *s, jnx_char *group,\
    jnx_char* port, jnx_uint8 *msg, jnx_size msg_len);


#ifdef __cplusplus
}
#endif
#endif /* !JNX_UDP_SOCKET_H */
