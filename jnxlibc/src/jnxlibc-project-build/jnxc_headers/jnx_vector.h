/**
 * @file jnxvector.h
 * @brief The API for jnxlibc's implementation of vector
 *
 * #include <jnxc_headers/jnxvector.h>
 */
#ifndef __JNX_VECTOR_H__
#define __JNX_VECTOR_H__
#include "jnx_types.h"
#include "jnx_thread.h"
#ifdef __cplusplus
extern "C" {
#endif
  /** 
   * @brief An internal implementation of record data
   */
  typedef struct{
    jnx_int32 used;
    void *data;
  }jnx_vector_record;
  /** 
   * @brief The external structure and interface for jnx vector
   */
  typedef struct{
    jnx_int32 count;
    jnx_vector_record **vector;
    jnx_thread_mutex *internal_lock;
  }jnx_vector;


  /**
   * @fn jnx_vector_create(void)
   * @brief Function that createialises internal vector structures. Call this function
   *			before you call any other function from the API.
   */
  jnx_vector *jnx_vector_create(void);

  /**
   * @fn jnx_vector_destroy(jnx_vector* vector)
   * @brief Deletes the vector and its data structures
   * @param vector pointer to the vector to destroy
   *
   * @warning User assumes responsibility for deleting all data entries first
   */
  void jnx_vector_destroy(jnx_vector** vector);

  /**
   * @fn jnx_vector_insert(jnx_vector *vector, void *value)
   * @brief inserts jnx_int32o the next position in the vector
   * @param vector pointer to the vector to insert int32_to
   * @param value value for insertion cast jnx_int32o void*
   */
  void jnx_vector_insert(jnx_vector *vector, void *value);

  void jnx_vector_insert_ts(jnx_vector *vector, void *value);
  /**
   * @fn jnx_vector_insert_at(jnx_vector *vector, jnx_int32 position, void *value)
   * @brief inserts jnx_int32o the vector at a particular position, if it doesn't exist it will be crearted
   * @param vector 
   * @param position
   * @param value
   */
  void jnx_vector_insert_at(jnx_vector *vector, jnx_int32 position, void *value);

  void jnx_vector_insert_at_ts(jnx_vector *vector, jnx_int32 position, void *value);
  /**
   * @fn jnx_vector_last(jnx_vector *vector)
   * @brief returns the last data entry in the vector and shortens it by 1
   * @param vector pointer to the vector to use
   *
   * @return returns void* data from the last vector entry
   */
  void *jnx_vector_last(jnx_vector *vector);

  void *jnx_vector_last_ts(jnx_vector *vector);

  /**
   * @fn jnx_vector_count(jnx_vector *vector);
   * @param vector is the pointer to the vector to count
   * @return number of vector elements
   */
  jnx_size jnx_vector_count(jnx_vector *vector);

  jnx_size jnx_vector_count_ts(jnx_vector *vector);

  /**
   * @fn jnx_vector_get_at(jnx_vector *vector, jnx_int32 position)
   * @brief getes an entry at position in the vector
   * @param vector pointer to the target vector
   * @param position target position
   * @return returns the value, returns NULL on error 
   */
  void* jnx_vector_get_at(jnx_vector *vector,jnx_int32 position);

  void* jnx_vector_get_at_ts(jnx_vector *vector,jnx_int32 position);
  /**
   * @fn jnx_vector_remove_at(jnx_vector *vector, jnx_int32 position)
   * @brief removes an entry at position in the vector
   * @param vector pointer to the target vector
   * @param position target position
   * @return returns the value, returns NULL on error 
   */
  void* jnx_vector_remove_at(jnx_vector *vector,jnx_int32 position);

  void* jnx_vector_remove_at_ts(jnx_vector *vector,jnx_int32 position);

  /**
   * @fn jnx_vector_contains(jnx_vector *vector, void *datain, jnx_int32(*vector_comparison)(void *a, void *b))
   * @brief checks whether a list contains a specific value
   * @param vector is a pointer to the jnx_vector_contains
   * @param datain iis the value to check the vector for
   * @param vector_comparison is the functor callback for comparison, 1 is a positive find, 0 is a negative
   * @return either 1 on success or 0 on failure
   */
  jnx_int32 jnx_vector_contains(jnx_vector *vector, void *datain, int32_t(*vector_comparison)(void *a,void *b));

  jnx_int32 jnx_vector_contains_ts(jnx_vector *vector, void *datain, int32_t(*vector_comparison)(void *a,void *b));
#ifdef __cplusplus
}
#endif
#endif 
