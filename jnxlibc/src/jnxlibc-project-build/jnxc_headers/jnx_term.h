/**
 * @file jnxterm.h
 * @brief terminal manipulation of colours, stdout and loading spinnders
 *
 * #include <jnxc_headers/jnxterm.h>
 */
#ifndef __JNX_TERM_H__
#define __JNX_TERM_H__
#include "jnx_types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define JNX_COL_BLACK   0
#define JNX_COL_RED     1
#define JNX_COL_GREEN   2
#define JNX_COL_YELLOW  3
#define JNX_COL_BLUE    4
#define JNX_COL_MAGENTA 5
#define JNX_COL_CYAN    6
#define JNX_COL_WHITE   7

  /**
   * @fn jnx_term_default
   * @brief resets to default terminal colors
   */
  void jnx_term_default();

  /**
   * @fn jnx_term_color(int fg_col)
   * @brief changes the terminal color 
   * @param fg_col select a color between 0-7 from JNX_COL
   */
  void jnx_term_color(jnx_int32 fg_col);

  /**
   * @fn jnx_term_printf_in_color(int fg_col, const char* format, ...)
   * @brief prints a single line in selected color
   * @param fg_col select a color between 0-7 from JNX_COL
   * @param format formatted string
   * @param ... formatted string optional arguments
   */
  void jnx_term_printf_in_color(jnx_int32 fg_col, const char* format, ...);
#ifdef __cplusplus
}
#endif
#endif
