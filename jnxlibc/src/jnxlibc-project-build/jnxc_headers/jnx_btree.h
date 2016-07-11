/** @file jnxbtree.h
 * @brief The API for jnxlibc's implementation of B-trees.
 *
 * #include <jnxc_headers/jnxbtree.h>
 */

#ifndef __JNX_B_TREE__
#define __JNX_B_TREE__
#ifdef __cplusplus
extern "C" {
#endif
#include "jnx_types.h"
#include "jnx_list.h"
#include "jnx_thread.h"
  /**
   * @brief Internal representation of a (key, value) pair.
   */
  typedef struct B_tree_record {
    void *key; /**< The key for the record. */
    void *value; /**< The value for the record.  */
  } record;


  /**
   * @brief Type for the callback function to determine the order of two keys.
   *
   * Callback function that the user needs to supply and which compares two keys,
   * and effectivelly two records as keys and values live together in a node record.
   *
   * The B-tree keys are required to follow some order, so with respect to that order
   * the function should return:
   *     - an integer < 0 if first parameter comes before second parameter
   *     - a 0 if first and second parameters are equal
   *     - an integer > 0 if the first parameter comes after the second parameter
   * 
   * Typically, these values can be -1, 0, and 1 respectivelly.
   */
  typedef jnx_int32 (*compare_keys) (void*, void*);


  /**
   * @brief Internal representation of a node in a B-tree.
   */
  typedef struct B_tree_node {
    jnx_int32 count; /**< How many records are in the node. */
    jnx_int32 is_leaf; /**< Is it a leaf node (set to 1) or an internal node (set to 0). */
    record **records; /**< The records contained in the node. */
    struct B_tree_node **children; /**< The subtrees of the node. A leaf node has each of its subtrees set to NULL. */
  } jnx_btree_node;


  /**
   * @brief Internal representation of the whole B-tree.
   */
  typedef struct B_tree {
    jnx_int32 order; /**< The order of the tree, i.e. the minimum number of records a non-root node can have. */
    compare_keys compare_function; /**< Callback function to compare keys. @see compare_keys */
    jnx_btree_node *root; /**< The root node of the B-tree. */
    jnx_thread_mutex *internal_lock;
  } jnx_btree;


  /** @fn jnx_btree* jnx_btree_init(int order, compare_keys callback)
   * @brief Create and initialise a new B-tree. This function must be called first, and once
   * per tree, before any other tree operations are called.
   *
   * Initialise B-tree with order and callback function which compares the keys of two records.
   *
   * @param order The order of the tree, i.e. the minumum number of records held in a 
   *                  non-root node.
   * @param callback The function that takes to keys as void pointers, and returns an
   *                     integer which represents whether the first key is "before", "equal"
   *                     or "after" the second key according to some key ordering. The function
   *                     returns an integer less than 0 if first key is before second key,
   *                     zero if both keys are equal, and an integer greater than 0 if the
   *                     first key is after the second key.
   *
   * @return The function returns a pointer to the new empty B-tree.
   */
  jnx_btree* jnx_btree_create(jnx_int32 order, compare_keys callback);

  /** @fn void jnx_btree_add(jnx_btree *tree, void *key, void *value)
   * @brief Add a record (key, value) to the B-tree.
   *
   * This function takes a key and a value for the record, creates the appropriate record
   * and inserts it into the tree. If a record with the same key already exists in the tree
   * the function will just replace the value for that record.
   *
   * @param tree The B-tree to which new record is added
   * @param key The key for the record
   * @param value The value for the record
   *
   * @warning The key and value are not copied they are just used as pointer assignments,
   * so it's important to think about the lifecycle of keys and values you pass to the function.
   */
  void jnx_btree_add(jnx_btree *tree, void *key, void *value);

  void jnx_btree_add_ts(jnx_btree *tree, void *key, void *value);
  /** @fn void jnx_btree_lookup(jnx_btree *tree, void *key)
   * @brief Retreive the value for key from the B-tree.
   *
   * The function returns the pointer to the value, or NULL if the key is not in the B-tree.
   *
   * @param tree The B-tree in which to look up the value for key.
   * @param key The key for lookup.
   * 
   * @return Pointer to the value, or NULL if key is not in the tree.
   *
   * @warning The value is just a pointer to, not a copy of the actual value so it is very important
   * to think carefully before you free its memory as it'll be freed from the B-tree record also.
   */
  void *jnx_btree_lookup(jnx_btree *tree, void *key);


  void *jnx_btree_lookup_ts(jnx_btree *tree, void *key);
  /** @fn void* jnx_btree_remove(jnx_btree *tree, void *key_in, void **key_out, void **val_out)
   * @brief Remove from the B-tree the record which matches the passed in key.
   *
   * If the record with @a key is in the @a tree, the record is removed and
   * the tree is rebalanced if necessary. If the record with @a key
   * is not in the tree, the @a tree is unchanged.
   *
   * @param tree The B-tree from which to remove the record.
   * @param key_in The key for which to remove the record.
   * @param key_out The reference by which the pointer to the memory for key from the tree record
   *					is returned. Set this to NULL if your keys are not malloced.
   * @param val_out The reference by which the pointer to the memory for value from the tree record
   *					is returned. Set this to NULL if your values are not malloced.
   *
   * @return Pointer to the value from the tree so that users can free potentially malloc'd values.
   *
   * @warning This operation will clean up the internal structures but will not attempt to free
   * the memory for the key and value of the deleted record. It's worth bearing this in mind to
   * avoid memory leaks.
   */
  void jnx_btree_remove(jnx_btree *tree, void *key_in, void **key_out, void **val_out);


  void jnx_btree_remove_ts(jnx_btree *tree, void *key_in, void **key_out, void **val_out);
  /** @fn void jnx_btree_delete(jnx_btree *tree)
   * @brief Delete the B-tree.
   *
   * Delete all the nodes and records of the B-tree. Upon function return the tree will be empty.
   *
   * @param tree The B-tree to delete.
   */
  void jnx_btree_destroy(jnx_btree* tree);
  /** @fh void jnx_btree_keys(jnx_btree *tree, jnx_list *keys)
   * @brief Put all the keys currently in the tree into the keys list.
   *
   * The function traverses the tree and puts all of the keys into the supplied
   * jnx_list @a keys. The @a keys list is ordered in the ascending order.
   * The @a keys list must not be NULL, otherwise the function won't traverse the tree. 
   *
   * @param tree The B-tree to traverse.
   * @param keys The jnx_list into which to put all of the @a tree keys.
   *
   * @warning Do not free key pointers from @a keys list, otherwise you'll corrupt
   * the B-tree. If you want the key removed use @a jnx_btree_remove function.
   */
  void jnx_btree_keys(jnx_btree *tree, jnx_list *keys);

  void jnx_btree_keys_ts(jnx_btree *tree, jnx_list *keys);
#ifdef __cplusplus
}
#endif	
#endif // __JNX_B_TREE__
