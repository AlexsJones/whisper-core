/*
 * =====================================================================================
 *
 *       Filename:  jnxsocket.h
 *
 *    Description:  Low level network API
 *
 *        Version:  1.0
 *        Created:  02/20/14 12:50:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
/** @file jnxsocket.h
 *  @brief The API for jnxlibc networking (includes TCP, UDP )
 *
 *  #include <jnxc_headers/jnxsocket.h>
 */
#ifndef __JNXSOCKET_H__
#define __JNXSOCKET_H__
#include "jnx_types.h"
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#ifdef __cplusplus
extern "C" {
#endif
#define KILOBYTE 1024
#define MAXBUFFER KILOBYTE * 10 
#define MAX_UDP_BUFFER KILOBYTE * 65
  typedef struct
  {
    jnx_int32 isclosed;
    jnx_int32 isconnected;
    jnx_int32 socket;
    jnx_unsigned_int addrfamily;
    jnx_size stype;
    jnx_char *ipaddress;
  } jnx_socket;
 
jnx_socket *create_socket(jnx_unsigned_int type,\
  jnx_unsigned_int addrfamily, jnx_unsigned_int protocol);
/**
   *@fn jnx_socket_socket_close(jnx_socket *s)
   *@brief close the socket but maintain the socket object
   *@params s the socket object to close
   */
  void jnx_socket_close(jnx_socket *s);
  /**
   * @fn void jnx_socket_socket_destroy(jnx_socket *s)
   * @param s is the socket to destroy
   */
  void jnx_socket_destroy(jnx_socket **s);

#ifdef __cplusplus
}
#endif
#endif
