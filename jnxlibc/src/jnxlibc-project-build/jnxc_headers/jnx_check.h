/*
 * =============================================================================
 *
 *       Filename:  jnxcheck.h
 *
 *    Description:  This gives the option to disable assertions in release mode
 *
 *        Version:  1.0
 *        Created:  05/08/14 07:57:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =============================================================================
 */
#ifndef __JNX_CHECK_H__
#define __JNX_CHECK_H__
#include <stdio.h>
#include <setjmp.h>
#include "jnx_types.h"
#include "jnx_log.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MAXSTACKFRAMES 20

  void jnxcheck_backtrace(void);

#if !defined(RELEASE)
#define JNXCHECK(X)\
  do\
  {\
    if(!(X)) {\
      JNXLOG(LERROR,"CHECK FAILED: %s [%s:%s:%d]",#X,__FILE__,__FUNCTION__,__LINE__);\
      jnxcheck_backtrace();\
      exit(1);\
    }\
  }\
  while(0) 
#else
#define JNXCHECK(X)\
  do { (void)sizeof(X); } while(0)
#endif

#define JNXFAIL(X)\
  do\
  {\
    JNXLOG(LERROR,"CHECK FAILED: %s [%s:%s:%d]",#X,__FILE__,__FUNCTION__,__LINE__);\
    jnxcheck_backtrace();\
    exit(1);\
  }\
  while(0)
  /*
   * @brief: JNX_EXCEPTION is the default case for exceptions when using try/catch
   *         This exception can also be replaced with -1
   *
   * @warning: The general rule is user created exceptions will decrement in value 
   */
#define JNX_EXCEPTION (-1)
  /*
   * @brief: To create a try/catch block jnx_try must be followed by jnx_try_end
   * @example:
   *      jnx_try {
   *
   *      }jnx_catch( JNX_EXCEPTION ) {
   *
   *      }jnx_finally {
   *
   *      }
   *      jnx_try_end
   */
#define jnx_try do{ jmp_buf ex_buf__; \
  switch( setjmp(ex_buf__) )\
  { case 0: while(1){ 

#define jnx_catch(x) break; case x:

#define jnx_finally break; } default:{

#define jnx_try_end } } }while(0);
  /*
   * @brief: jnx_throw can be used at any point to jump to the last setjmp point
   */
#define jnx_throw(x) longjmp(ex_buf__, x)

#ifdef __cplusplus
}
#endif
#endif
