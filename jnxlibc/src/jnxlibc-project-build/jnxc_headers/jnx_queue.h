/*
 * =====================================================================================
 *
 *       Filename:  jnxqueue.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/27/14 13:50:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __JNX_QUEUE__
#define __JNX_QUEUE__
#ifdef __cplusplus
extern "C" {
#endif
#include "jnx_types.h"
#include "jnx_list.h"
#include "jnx_thread.h"
  typedef struct jnx_queue {
    jnx_list *list;
    jnx_thread_mutex *internal_lock;
  }jnx_queue;

  /**
   * @fn void jnx_queue_push(jnx_queue *q, void *data)
   * @brief pushes an item onto the queue
   * @param q is a pointer to the queue to use
   * @param data is the data to store
   */
  void jnx_queue_push(jnx_queue *q, void *data);

  void jnx_queue_push_ts(jnx_queue *q, void *data);
  /**
   * @fn void* jnx_queue_pop(jnx_queue *q)
   * @brief pops the first available item off of the queue
   * @param q is a pointer to the queue to use
   * @return the first queue item or NULL on error 
   */
  void *jnx_queue_pop(jnx_queue *q);

  void *jnx_queue_pop_ts(jnx_queue *q);
  /**
   * @fn size_t jnx_queue_count(jnx_queue *q)
   * @brief get the queue size
   * @param q is a pointer to the queue to use
   * @return the size of queue, 0 on empty or error
   */
  size_t jnx_queue_count(jnx_queue *q);

  size_t jnx_queue_count_ts(jnx_queue *q);
  /**
   * @fn jnx_queue* jnx_queue_create()
   * @brief create a jnx_queue
   * @return a pointer to jnx_queue
   */
  jnx_queue *jnx_queue_create();
  /**
   * @fn void jnx_queue_destroy(jnx_queue **q)
   * @brief destroys a jnx_queue
   * @param q is a pointer to the queue to use
   */
  void jnx_queue_destroy(jnx_queue **q);

#ifdef __cplusplus
}
#endif 
#endif
