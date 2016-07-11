/** @file jnxquicksort.h
 * @brief The API for sorting arrays of void* data items that can be ordered, 
 * via Quicksort algorithm with median-of-three pivot choice strategy.
 */

#ifndef __JNXQUICKSORT_H__
#define __JNXQUICKSORT_H__
#include "jnx_types.h"
#ifdef __cplusplus
extern		"C" {
#endif
  typedef jnx_int32(*compare_function)(void *, void *);

  /*
   * @fn jnx_quicksort(void **array, jnx_int32 size, compare_function cf);
   * @brief Sorts the array based on the comparison function cf.
   * @param array - the array to sort, it should store data as void * pointers
   * or be cast into that type if the data structure allows it.
   * @param size - the size of the array
   * @param cf - compare function that takes two void *pointers, which will be
   * elements of the array, and returns < 0 if first is "smaller" than second, 
   * 0 if they're equal, or > 0 if first is "larger" than second.
   *
   * The array is sorted in place and the original array will contain elements
   * in sorted order according to the order imposed by cf.
   */
  void jnx_quicksort(void **array, jnx_int32 size, compare_function cf);

#ifdef __cplusplus
}
#endif
#endif
