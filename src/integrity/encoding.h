/*
 * =====================================================================================
 *
 *       Filename:  encoding.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/20/2015 08:01:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __ENCODING_H__
#define __ENCODING_H__
#include <jnxc_headers/jnx_types.h>
jnx_uint8 *encode_from_string(jnx_uint8 *input, jnx_size input_length,
    jnx_size *output_length);

jnx_char *decode_to_string(jnx_uint8 *input, jnx_size input_length,
    jnx_size *output_length);

#endif
