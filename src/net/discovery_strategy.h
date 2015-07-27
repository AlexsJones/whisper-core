/*
 * =====================================================================================
 *
 *       Filename:  discovery_strategy.h
 *
 *    Description:  Struct and accompanying functions that define the API for discovery
 *                  implementations. The whisper-core comes with pre-canned strategies
 *                  for discovering peers on the LAN and across the LAN boundaries, but
 *                  users can implement their own in the shared libraries and specify
 *                  them to be loaded at run time as plugins.
 *
 *        Version:  1.0
 *        Created:  26/07/2015 08:50:06
 *       Revision:  none
 *       Compiler:  gcc,lldb
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __DISCOVERY_STRATEGY_H__
#define __DISCOVERY_STRATEGY_H__

#include <jnxc_headers/jnxtypes.h>
#include "peer.h"
#include "discovery.h"

typedef jnx_int32 (*start_callback)(discovery_service *);
typedef jnx_int32 (*stop_callback)(void);
typedef jnx_int32 (*active_peer_callback)(peer *);

typedef struct {
  start_callback start;
  stop_callback stop;
  active_peer_callback is_active_peer;

} discovery_strategy;

#endif // __DISCOVERY_STRATEGY_H__
