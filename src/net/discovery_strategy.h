/*
 * =============================================================================
 *
 *       Filename:  discovery_strategy.h
 *
 *    Description:  Structure and accompanying functions that define the API for
 *                  discovery implementations.
 *
 *                  whisper-core comes with default strategies for discovering
 *                  peers on the LAN and across the LAN boundaries, but users
 *                  can implement their own as shared libraries and specify
 *                  them to be loaded as plugins at run time.
 *
 *        Version:  1.0
 *        Created:  26/07/2015 08:50:06
 *       Revision:  none
 *       Compiler:  gcc,lldb
 *
 *         Author:  Dragan Glumac (DG), 
 *   Organization:  
 *
 * =============================================================================
 */

#ifndef __DISCOVERY_STRATEGY_H__
#define __DISCOVERY_STRATEGY_H__

#include <jnxc_headers/jnxtypes.h>
#include "peer.h"
#include "discovery.h"

/**
 * @typedef start_t
 * @brief Function that is expected to start the peer discovery process.
 *
 * @parameter discovery_service which will use this strategy.
 * @return 0 for success, non-zero for failure.
 */
typedef jnx_int32 (*start_t)(discovery_service *);

/**
 * @typedef stop_t
 * @brief Function that stops the peer discovery process.
 *
 * @return 0 for success, non-zero for failure.
 */
typedef jnx_int32 (*stop_t)(void);

/**
 * @typedef active_peer_t
 * @brief Function that determines if a peer is active.
 *
 * This is entirely dependent on the strategy implementation. For example,
 * if the strategy expects a peer to announce its presence every 10 seconds
 * then the peer is deemed active if the last time it was seen is within
 * 10 seconds of now.
 *
 * @param peer in question
 * @return 1 for active peer, 0 for non active peer.
 */
typedef jnx_int32 (*active_peer_t)(peer *);

/**
 * @typedef my_address_t
 * @brief Function that returns string representation of local address
 * as seen by other peers, e.g. IP address or DNS name etc.
 *
 * @return string representation of the address
 */
typedef char *(*my_address_t)(void);

/**
 * @struct discovery_strategy
 * @brief API for peer discovery implementations.
 *
 * Strategies are used by the discovery_service to continuously maintain
 * the collection of active peers in the peerstore of the discovery_service.
 *
 * whisper-core comes with pre-defined strategies for LAN and across-network
 * discovery, but should the user wish to implement their own it should be
 * packaged as a dynamically loaded library plugin (<name>.dylib on Mac OS X,
 * or <name>.so on Linux).
 *
 * If you're implementing your own discovery strategy, the discovery service
 * will expect to find an initialisation function with the following signature:
 *
 * \code{.c}
 *   discovery_strategy *discovery_strategy_init(void);
 * \endcode
 *
 * This function is the ideal place to assign the function pointers of the
 * discovery_service struct to the corresponding implementation routines.
 *
 * NOTE: If you're overriding the default struct discovery_strategy with
 * an extended one, then you must:
 *
 *   (1) lay out the first 5 fields of your struct with exactly the same
 *       names and in exactly the same order as discovery_service struct, and
 *   (2) all extra fields should come after the discovery_service fields.
 *
 * The discovery_service will call start_discovery as part of its startup
 * routine, and it will call stop_discovery as part of its stop and
 * destroy routines.
 *
 * The API makes no assumptions about start and stop routines being re-entrant,
 * so it is left as an implementation detail of the plugin.
 *
 * is_active_peer is called at various times, but always in a read-only
 * fashion so it does not need to be thread-safe.
 *
 * get_my_address is expected to return a string representation of network
 * address for the local machine, but as seen by the other peers. So when you
 * are implementing a strategy that crosses LAN boundaries you will probably
 * want to return an IP address that is visible to the outside world.
 */
typedef struct {
  // discovery service which uses this strategy
  discovery_service *service;

  // start discovering peers
  start_t start_discovery;

  // stop discovering peers
  stop_t stop_discovery;

  // determine if a peer is active or not
  active_peer_t is_active_peer;

  // get my ip address as seen by other peers
  my_address_t get_my_address;

} discovery_strategy;

#endif // __DISCOVERY_STRATEGY_H__
