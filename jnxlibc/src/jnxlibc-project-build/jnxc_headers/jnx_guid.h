/*
 * =====================================================================================
 *
 *       Filename:  jnxguid.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  26/01/2015 21:16:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (jonesax@hush.com)
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __JNXGUID_H__
#define __JNXGUID_H__
#include "jnx_types.h"
#include "jnx_file.h"
#ifdef __cplusplus
extern		"C" {
#endif
  typedef enum jnx_guid_state {
    JNX_GUID_STATE_SUCCESS,
    JNX_GUID_STATE_FAILURE
  }jnx_guid_state;

  typedef struct jnx_guid {
    jnx_uint8 guid[16];
  }jnx_guid;

  /**@fn jnx_guid_state jnx_guid_create(jnx_guid *guid)
   * @brief generates a guid based off of /dev/urandom noise
   * @param guid can be a pointer from a stack intialized structure
   * @return jnx_guid_state JNX_GUID_STATE_SUCCESS or JNX_GUID_STATE_FAILURE
   */
  jnx_guid_state jnx_guid_create(jnx_guid *guid);

  /**@fn jnx_guid_compare(jnx_guid *ga, jnx_guid *gb);
   * @brief compares two guids and on success returns JNX_GUID_STATE_SUCCESS
   * @param ga the first guid
   * @param gb the second guid to compares
   * @return jnx_guid_state
   */
  jnx_guid_state jnx_guid_compare(jnx_guid *ga, jnx_guid *gb); 

  /**@fn jnx_guid_compare_raw(jnx_uint8 *ga, jnx_uint8 *gb);
   * @brief compares two guids and on success returns JNX_GUID_STATE_SUCCESS
   * @param ga the first array of jnx_uint8
   * @param gb the second array of jnx_uint8
   * @return jnx_guid_state
   */
  jnx_guid_state jnx_guid_compare_raw(jnx_uint8 *ga, jnx_uint8 *gb); 
  /**@fn jnx_guid_to_string(jnx_guid *guid,jnx_char **outstr);
   * @brief populates a string from a guid structure
   * @param guid is a structure to retrieve the guid string from
   * @outstr is a double pointer to null string to pass in
   */
  void jnx_guid_to_string(jnx_guid *guid,jnx_char **outstr);
  /**@fn jnx_guid_from_string(jnx_char *str, jnx_guid *guid);
   * @brief converts a 32 byte string into a 16 byte guid
   * @warning must not contains dashes, just hex decimal
   * @param str hexdecimal string for conversion
   * @param oguid out ref pointer for populated struct
   */
  void jnx_guid_from_string(jnx_char *str, jnx_guid *oguid);
#ifdef __cplusplus
}
#endif
#endif
