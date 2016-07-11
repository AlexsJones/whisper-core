/*
 * jnx_tcp_socket.h
 * Copyright (C) 2015 Dragan Glumac <dragan.glumac@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef JNX_IPC_SOCKET_H
#define JNX_IPC_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "jnx_check.h"
#include "jnx_types.h"
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct sockaddr_un jnx_ipc_socket_address;

typedef struct {
  jnx_uint32 isclosed;
  jnx_uint32 islisten;
  jnx_uint32 isconnected;
  jnx_int32 socket;
  jnx_ipc_socket_address address;
} jnx_ipc_socket;

typedef struct jnx_ipc_listener {
  jnx_ipc_socket *socket;
  struct pollfd ufds[200];
  jnx_int nfds;
  jnx_int poll_timeout;
  jnx_int hint_exit;
} jnx_ipc_listener;

typedef void (*jnx_ipc_listener_callback)(const jnx_uint8 *payload,
                                          jnx_size bytes_read,
                                          int connected_socket,
                                          void *args);

/**
 * @fn jnx_ipc_socket *jnx_socket_ipc_create(jnx_char *socket_path)
 * @brief creates a jnx inter-process communication (AF_UNIX) socket
 * @param socket_path - the path on the file system to use for the Unix socket
 * @return jnx_ipc_socket
 */
jnx_ipc_socket *jnx_socket_ipc_create(jnx_char *socket_path);

/**
 * @fn void jnx_ipc_socket_destroy(jnx_ipc_socket **s)
 * @brief destroys and cleans up a jnx IPC socket
 * @param s - the socket to destroy
 */
void jnx_ipc_socket_destroy(jnx_ipc_socket **s);

/**
 * @fn jnx_ipc_listener *jnx_socket_ipc_listener_create(char *port,
 unsigned int family, int max_connections);
 * @brief creates a non-blocking multiplexing IPC socket server
 * that needs to be ticked
 * @param s -  IPC socket on which to listen
 * @param max_connections indicate the size
 * of listener backlog (must not exceed nfds file descriptors)
 * @return jnx_ipc_listener
 */
jnx_ipc_listener *jnx_socket_ipc_listener_create(jnx_ipc_socket *s,
                                                 jnx_int max_connections);

/**
 *@fn void jnx_socket_ipc_listener_destroy(jnx_ipc_listener **listener)
 *@brief destroys and closes socket for the tcp listener
 *@param listener is a pointer to pointer of the current listener
 */
void jnx_socket_ipc_listener_destroy(jnx_ipc_listener **listener);

/**
 *@fn jnx_socket_ipc_listener_tick(jnx_ipc_listener *listener,
 * jnx_ipc_listener_callback, void *args)
 *@brief tick will need to be called manually by the user to accept and recv
 *incoming network data. It has been designed for use in a loop
 *and to provide max user control
 *@param listener is the current instantiated listener
 *@param callback is passed into the tick representing the function
 *to return received data too
 *@param args are the context arguments to pass to the receiver function
 *can be null
 */
void jnx_socket_ipc_listener_tick(jnx_ipc_listener *listener,
                                  jnx_ipc_listener_callback callback,
                                  void *args);


/**
 *@fn jnx_socket_ipc_listener_tick(jnx_ipc_listener *listener,
 * jnx_ipc_listener_callback, void *args)
 *@brief auto tick will block and loop automatically for the user
 *user to accept and recv incoming network data.
 *@warning BLOCKING
 *@param listener is the current instantiated listener
 *@param callback is passed into the tick representing the function
 *to return received data too
 *@param args are the context arguments to pass to the receiver function
 *can be null
 */
void jnx_socket_ipc_listener_auto_tick(jnx_ipc_listener *listener,
                                       jnx_ipc_listener_callback callback,
                                       void *args);

/**
 * @fn jnx_socket_ipc_send((jnx_socket *s, jnx_char *host, jnx_char* port,
 jnx_uint8 *msg, jnx_size msg_len)
 * @param s is the socket to use to send
 * @param msg is the payload to send
 * @param msg_len is the size of payload
 * @return jnx_size of bytes sent
 */
jnx_size jnx_socket_ipc_send(jnx_ipc_socket *s,
                             jnx_uint8 *msg, jnx_size msg_len);

/**
 * @fn jnx_socket_ipc_send_with_receipt(jnx_socket *s, jnx_char *host,
 jnx_char* port, jnx_uint8 *msg, jnx_size msg_len,jnx_uint8 **out_receipt)
 * @param s is the socket to use to send
 * @param msg is the payload to send
 * @param msg_len is the size of payload
 * @param out_receipt returned from the target host
 * @return jnx_size of bytes received
 */
jnx_size jnx_socket_ipc_send_with_receipt(jnx_ipc_socket *s,
                                          jnx_uint8 *msg, jnx_size msg_len,
                                          jnx_uint8 **out_receipt);

#ifdef __cplusplus
}
#endif
#endif /* !JNX_IPC_SOCKET_H */
