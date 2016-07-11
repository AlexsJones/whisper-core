/*
 * =====================================================================================
 *
 *       Filename:  jnxsignal.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/14 17:02:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __JNX_SIGNAL_H__
#define __JNX_SIGNAL_H__
#include "jnx_types.h"
#include "jnx_list.h"
#include "jnx_thread.h"
#ifdef __cplusplus
extern "C" {
#endif
  typedef struct jnx_signal {
    jnx_list *slot_list;
    void *data;
    jnx_thread_mutex *internal_lock;
  }jnx_signal;
  /*
   * @brief the function definition for a slot
   * @param s is the signal calling the slot, also gets passed in
   */
  typedef void jnx_slot(jnx_signal *s);
  /* @fn jnx_signal *jnx_signal_create()
   * @brief Creates a signal 
   * @return returns a pointer to the newly created signal
   */
  jnx_signal *jnx_signal_create();
  /*@fn jnx_signal_destroy
   *@brief destroys the signal passed in
   *@param s is a ptr to ptr of signal (becomes NULL)
   */
  void jnx_signal_destroy(jnx_signal **s);
  /*@fn jnx_signal_connect(jnx_signal *s, jnx_slot)
   *@brief this can be called multiple times per signal and adds slots to them
   *@param s is the signal pointer to use for associating
   *@param jnx_slot is the function pointer to pass in to use as a slot
   */
  void jnx_signal_connect(jnx_signal *s, jnx_slot);
  /*@fn jnx_signal_call(jnx_signal *s, void *data)
   *@brief Calls all slots on a signal
   *@param s is the signal pointer to act as caller
   *@param data is an optional parameter of data to pass to slots
   */
  jnx_int32 jnx_signal_call(jnx_signal *s,void *data);
#ifdef __cplusplus
}
#endif
#endif
