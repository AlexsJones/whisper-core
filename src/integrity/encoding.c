/*
 * =====================================================================================
 *
 *       Filename:  encoding.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/20/2015 08:02:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "encoding.h"
#include <jnxc_headers/jnxencoder.h>

jnx_uint8 *encode_from_string(jnx_uint8 *input, jnx_size input_length,
    jnx_size *output_length) {
  jnx_encoder *e = jnx_encoder_create();
  jnx_uint8 *encoded = jnx_encoder_b64_encode(e,input,input_length,output_length);
  jnx_encoder_destroy(&e);
  return encoded;
}
jnx_char *decode_to_string(jnx_uint8 *input, jnx_size input_length,
    jnx_size *output_length) {
  jnx_encoder *e = jnx_encoder_create();
  jnx_char *decoded = (jnx_char*)jnx_encoder_b64_decode(e,input,input_length,output_length);
  jnx_encoder_destroy(&e);
  return decoded;
}
