/*
 * jnx_tcp_socket.h
 * Copyright (C) 2015 tibbar <tibbar@debian>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef JNX_TCP_SOCKET_H
#define JNX_TCP_SOCKET_H
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "jnx_check.h"
#include "jnx_socket.h"
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct jnx_tcp_listener {
    jnx_socket *socket;
    struct pollfd ufds[200];
    jnx_int nfds;
    jnx_int poll_timeout;
    jnx_int hint_exit;
  }jnx_tcp_listener;

  typedef void (*jnx_tcp_listener_callback)(const jnx_uint8 *payload, \
      jnx_size bytes_read, int connected_socket, void *args);

  /**
   * @fn jnx_socket *jnx_socket_tcp_create(jnx_unsigned_int addrfamily)
   * @brief creates a jnx tcp socket
   * @param addrfamily this is the address family to use (e.g. AF_INET)
   * @return jnx_socket
   */
  jnx_socket *jnx_socket_tcp_create(jnx_unsigned_int family);
  /**
   * @fn jnx_tcp_listener *jnx_socket_tcp_listener_create(char *port,
   unsigned int family, int max_connections);
   * @brief creates a non-blocking 
   * async multiplexing TCP server that needs to be ticked
   * @param port is the listening port to bind too locally
   * @param family is the address family type (e.g. AF_INET,AF_INET6)
   * @param max_connections indicate the size 
   * of listener backlog (must not exceed ufds fd count)
   * @return jnx_tcp_listener
   */
  jnx_tcp_listener* jnx_socket_tcp_listener_create(jnx_char *port,
      jnx_unsigned_int family, jnx_int max_connections);

  /**
   *@fn void jnx_socket_tcp_listener_destroy(jnx_tcp_listener **listener)
   *@brief destroys and closes socket for the tcp listener
   *@param listener is a pointer to pointer of the current listener
   */
  void jnx_socket_tcp_listener_destroy(jnx_tcp_listener **listener);
  /**
   *@fn jnx_socket_tcp_listener_tick(jnx_tcp_listener *listener,
   * jnx_tcp_listener_callback, void *args)
   *@brief tick will need to be called manually by the user to accept and recv
   *incoming network data. It has been designed for use in a loop 
   *and to provide max user control
   *@param listener is the current instantiated listener
   *@param callback is passed into the tick representing the function
   *to return received data too
   *@param args are the context arguments to pass to the receiver function 
   *can be null
   */   
  void jnx_socket_tcp_listener_tick(jnx_tcp_listener* listener,
      jnx_tcp_listener_callback callback, void *args);

  /**
   *@fn jnx_socket_tcp_listener_tick(jnx_tcp_listener *listener,
   * jnx_tcp_listener_callback, void *args)
   *@brief auto tick will block and loop automatically for the user
   *user to accept and recv incoming network data.
   *@warning BLOCKING
   *@param listener is the current instantiated listener
   *@param callback is passed into the tick representing the function
   *to return received data too
   *@param args are the context arguments to pass to the receiver function 
   *can be null
   */
  void jnx_socket_tcp_listener_auto_tick(jnx_tcp_listener *listener,\
      jnx_tcp_listener_callback callback, void *args);

  jnx_char *jnx_socket_tcp_resolve_ipaddress(jnx_int32 socket);
  /**
   * @fn jnx_socket_tcp_send((jnx_socket *s, jnx_char *host, jnx_char* port,
   jnx_uint8 *msg, jnx_size msg_len)
   * @param s is the socket to use to send
   * @param host is the target destination
   * @param port is the target port
   * @param msg is the payload to send
   * @param msg_len is the size of payload
   * @return jnx_size of bytes sent
   */
  jnx_size jnx_socket_tcp_send(jnx_socket *s, jnx_char *host,\
      jnx_char* port, jnx_uint8 *msg, jnx_size msg_len);
  /**
   * @fn jnx_socket_tcp_send_with_receipt(jnx_socket *s, jnx_char *host,
   jnx_char* port, jnx_uint8 *msg, jnx_size msg_len,jnx_uint8 **out_receipt)
   * @param s is the socket to use to send
   * @param host is the target destination
   * @param port is the target port
   * @param msg is the payload to send
   * @param msg_len is the size of payload
   * @param out_receipt returned from the target host
   * @return jnx_size of bytes sent
   */
  jnx_size jnx_socket_tcp_send_with_receipt(jnx_socket *s,\
      jnx_char *host, jnx_char* port, jnx_uint8 *msg, \
      jnx_size msg_len,jnx_uint8 **out_receipt);
#ifdef __cplusplus
}
#endif
#endif /* !JNX_TCP_SOCKET_H */
