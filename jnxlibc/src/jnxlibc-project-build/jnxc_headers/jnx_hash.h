/** @file jnxhash.h
 *  @brief API for jnxlibc's implementation of hashmap
 *
 *  #include <jnxc_headers/jnxhash.h>
 */

#ifndef __JNX_HASH_H__
#define __JNX_HASH_H__
#include <string.h>
#include <stdlib.h>
#include "jnx_check.h"
#include "jnx_types.h"
#include "jnx_list.h"
#include "jnx_thread.h"
#ifdef __cplusplus
extern "C" {
#endif
  /**
   * @brief Internal container for a key value pair
   */
  typedef struct bucketel {
    jnx_char* origin_key;
    const jnx_char* origin_value;
  } jnx_hash_bucket_el;

  /**
   * @brief Internal container for key value pair lists
   */
  typedef struct hashel {
    jnx_list* bucket;
    jnx_int32 bucket_len;
    jnx_int32 used;
  } jnx_hash_element;

  /* *
   * @brief External facing representaion of the hashmap
   */
  typedef struct hashmap {
    jnx_hash_element* data;
    jnx_unsigned_int size;
    jnx_unsigned_int used_up;
    jnx_thread_mutex *internal_lock;
  } jnx_hashmap;

  /** @fn jnx_hash_destroy(jnx_hashmap* hashmap)
   *  @brief Deletes the keys and the hashmap 
   *  @warning Does not remove values 
   *  @param hashmap pointer to the map to be deleted
   */
  void jnx_hash_destroy(jnx_hashmap** hashmap);

  void jnx_hash_destroy_ts(jnx_hashmap* hashmap);
  /** @fn jnx_hash_delete_value(jnx_hashmap *hashmap, jnx_char *key)
   *  @brief removes the key and returns the value for an entry
   *  @param hashmap pointer to the map 
   *  @param key pointer to key for lookup and removal
   *  @return pointer to value
   */
  void* jnx_hash_delete_value(jnx_hashmap* hashmap, jnx_char *key);

  void* jnx_hash_delete_value_ts(jnx_hashmap* hashmap, jnx_char *key);
  /** @fn jnx_hash_create(jnx_unsigned_int size)
   *  @brief Returns a newly created hashmap
   *  @param size createial size for the hashmap, jnx_unsigned_int i.e 1024
   *  @return newly created hashmap with size
   */
  jnx_hashmap* jnx_hash_create(jnx_unsigned_int size);

  /** @fn jnx_hash_put(jnx_hashmap* hashmap, const jnx_char *key, void *value)
   *  @brief Given a key and value this hashs and stores
   *  @param hashmap pointer to the target hashmap
   *  @param key the key you want to associate with the value
   *  @param value data
   */
  jnx_int32 jnx_hash_put(jnx_hashmap* hashmap, const jnx_char* key, void* value);

  jnx_int32 jnx_hash_put_ts(jnx_hashmap* hashmap, const jnx_char* key, void* value);
  /** @fn jnx_hash_get(jnx_hashmap* hashmap, const jnx_char *key)
   *  @brief Returns the value for a given key
   *  @param hashmap pointer to hashmap for retrieval
   *  @param key for search and value retrieval
   *  @return void pointer to the data from value
   */
  void* jnx_hash_get(jnx_hashmap* hashmap, const jnx_char* key);

  void* jnx_hash_get_ts(jnx_hashmap* hashmap, const jnx_char* key);
  /** @fn jnx_hash_get_keys(jnx_hashmap* hashmap, const jnx_char ***keys)
   *  @brief Returns the number of keys found, and mallocs keys array and inserts keys
   *  @param hashmap pointer to hashmap for retrieval
   *  @param keys is a pointer to an array which is given for mallocing and insertion
   *  @return number of keys found
   */
  jnx_int32 jnx_hash_get_keys(jnx_hashmap* hashmap,const jnx_char ***keys);

  jnx_int32 jnx_hash_get_keys_ts(jnx_hashmap* hashmap,const jnx_char ***keys);
  /** @fn jnx_hash_string(const jnx_char* input, int map_size) 
   *  @brief  Allows access to the hash string function, returning and jnx_unsigned_int
   *  @param input is the string to be hashed
   *  @param map_size is used to round the remainder of the string
   *  @return hash value of the input string
   */
  jnx_int32 jnx_hash_string(const jnx_char* input, jnx_int32 map_size);
#ifdef __cplusplus
}
#endif
#endif
