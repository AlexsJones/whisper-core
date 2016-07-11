/*
 * =====================================================================================
 *
 *       Filename:  jnxtypes.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/15 11:09:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __JNXTYPES_H__
#define __JNXTYPES_H__
#include <stdint.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif
  typedef char jnx_char; //8 bits
  typedef signed char jnx_signed_char;
  typedef unsigned char jnx_unsigned_char;

  typedef int jnx_int; //16 bits (at least)
  typedef signed int jnx_signed_int;
  typedef unsigned int jnx_unsigned_int;

  typedef unsigned long jnx_ulong;
  typedef long int jnx_long_int;

  typedef uint8_t jnx_uint8;
  typedef uint32_t jnx_uint32;
  typedef uint64_t jnx_uint64;

  typedef int32_t jnx_int32;

  typedef float jnx_float;

  typedef struct jnx_vector2D {
    jnx_float x;
    jnx_float y;
  }jnx_vector2D;

  typedef struct jnx_vector3D {
    jnx_float x;
    jnx_float y;
    jnx_float z;
  }jnx_vector3D;

  typedef size_t jnx_size;
  typedef ssize_t jnx_ssize;
  typedef double jnx_double;
#ifdef __cplusplus
}
#endif
#endif
