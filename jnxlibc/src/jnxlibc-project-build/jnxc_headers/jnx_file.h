/** @file jnxfile.h
 *  @brief File manipulation, read/write, kvp plucking and recursive deletion
 *
 *  #include <jnxc_headers/jnxfile.h>
 */
#ifndef __JNXFILE_H__
#define __JNXFILE_H__
#include <stddef.h>
#include "jnx_types.h"
#include "jnx_hash.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
 * In preparation for splitting out platform functions
 */
#define JNX_OPEN open
#define JNX_READ read
#define JNX_CLOSE close
  
  /**
   * @brief An jnx_int32ernal representation of the kvp node populated when 
   *			used in key value pair retrieval from a file
   */
  typedef struct jnx_file_kvp_node {
    jnx_char* key;
    jnx_char* value;
    struct jnx_file_kvp_node* next;
  }jnx_file_kvp_node;
  /**
   * @fn jnx_file_read(jnx_char* path, jnx_char **buffer,jnx_char *flags)
   * @brief Function to read the contents of a text file jnx_int32o the buffer.
   *
   * @param path path to the target file for reading
   * @param buffer that will be filled
   * @param flags are the flags passed (r,rb...)
   * @return Number of bytes read
   *
   * @warning it is the users responsibility to manage the memory of the jnx_char*
   */
  jnx_size jnx_file_read(jnx_char* path, jnx_char **buffer,jnx_char *flags);
  /**
   * @fn jnx_hashmap *jnx_file_read_kvp(jnx_char *path, jnx_size max_buffer,jnx_char *flags)
   * @brief Function to read a file that contains key-value pairs jnx_int32o a linked list.
   *
   * @param path path to the target file for reading
   * @param delimiter the token to break the key value pairs on e.g. '='
   * @param max_buffer is maximum line size buffer
   * @return returns a hasmap of jnx_file_kvp_nodes, i.e. key-value pairs.
   *
   * @warning user is responsible for hashmap deletion and its node/data
   */
  jnx_hashmap *jnx_file_read_kvp(jnx_char *path, jnx_size max_buffer, jnx_char *delimiter);

  /**
   * @fn jnx_file_write(jnx_char *path, jnx_char* data, jnx_size data_size)
   * @brief Function to write the data buffer jnx_int32o the text file point32_ted to by path.
   *
   * @param path path to the target file for writing
   * @param data pojnx_int32er to the jnx_char* for writing to the file
   * @param data_size size of data to be written
   * @param flags are the flags passed (w,r,a+ ...)
   * @return Number of bytes written on success
   */
  jnx_size jnx_file_write(jnx_char* path, jnx_char* data, jnx_size data_size,jnx_char *flags);
  /**
   * @fn jnx_file_recursive_delete(jnx_char* path, jnx_int32 depth)
   * @brief Function to recursively delete a directory pojnx_int32ed to by path.
   *
   * @param path the provided path will have all its sub directories and files deleted
   * @param depth defines how deep to recursively delete in a folder structure
   * @return success on 0
   *
   * @warning extremely powerful and can make unrepairable changes to your file system
   */
  jnx_int32 jnx_file_recursive_delete(jnx_char* path, int32_t depth);

  /**
   * @fn jnx_file_mktempdir(jnx_char *template, jnx_char **path) 
   * @brief Function to create a temporary directory on the given path
   * @param template is the path to create the directory on
   * @param path is a pojnx_int32er to the output buffer jnx_char point32_ter
   * @return success on 0
   */
  jnx_int32 jnx_file_mktempdir(jnx_char *dirtemplate, jnx_char **path);
  /**
   * @fn jnx_file_exists(jnx_char *file)
   * @brief checks if a file exists
   * @param filepath to check 
   * @returns 1 on file existance
   */
  jnx_int32 jnx_file_exists(jnx_char *file);
#ifdef __cplusplus
}
#endif
#endif
