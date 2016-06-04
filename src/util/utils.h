/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2015 01:32:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __UTILS_H__
#define __UTILS_H__
#include <jnxc_headers/jnx_guid.h>
#include "peer.h"
#include <jnxc_headers/jnx_log.h>
#include "cryptography.h"
void print_guid(jnx_guid *g);
void print_pair(jnx_guid *a, jnx_guid *b);
void print_peer(peer *p);
void print_public_key(RSA *remote_pub_keypair);
#endif
