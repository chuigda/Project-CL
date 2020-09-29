#ifndef CCLIB_CFG_TEMP_H
#define CCLIB_CFG_TEMP_H

/** Redefine CCLIB size type */
#define cc_size_t  size_t
#define cc_ssize_t ptrdiff_t

/** Feature implemenational choices
 *
 * User may provide implementation on themselves by appointing
 * CC_FEAT_USE_USER. User must provide the interfaces required by
 * header file in their own implementation source file.
 *
 * */
#define CC_FEAT_DISABLED  /* Disable this feature   */             0
#define CC_FEAT_USE_STD   /* Wrap C standard library */            1
#define CC_FEAT_USE_CCLIB /* Use CCLIB's own implementation */     2
#define CC_FEAT_USE_GLIB  /* Use GLIB to implement */              3
#define CC_FEAT_USE_USER  /* Let user to provide implementation */ 4

/** Memory allocation library
 * 
 * Possible choices: DISABLED, STD, CCLIB, USER
 *
 *   - If STD choosed, CCLIB will use malloc
 *   - If CCLIB choosed, CCLIB will use vkalloc
 *
 * */
#define CCLIB_ALLOC     CC_FEAT_USE_STD

/** String manipulation library
 * 
 * Possible choices: DISABLED, STD, CCLIB, USER
 *
 *   - If STD choosed, CCLIB will use <string.h>
 *   - If CCLIB choosed, CCLIB will use builtin implementation
 *
 * */
#define CCLIB_STRLIB    CC_FEAT_USE_STD

/** Dynamic multi-dimension array
 *
 * This feature is inaccessible if CCLIB_ALLOC is DISABLED
 * 
 * Possible choices: DISABLED, CCLIB, USER
 *
 * */
#define CCLIB_DS_DMARR  CC_FEAT_USE_CCLIB

/** Vector, the continuous storage container
 *
 * This feature is inaccessible if CCLIB_ALLOC is DISABLED
 *
 * Possible choices: DISABLED, CCLIB, USER
 *
 * */
#define CCLIB_DS_VEC    CC_FEAT_USE_CCLIB

/** Linked list, the linked container
 *
 * This feature is inaccessible if CCLIB_ALLOC is DISABLED
 *
 * Possible choices: DISABLED, CCLIB, GLIB, USER
 *
 * */
#define CCLIB_DS_LLIST  CC_FEAT_USE_CCLIB

/** Intrusive linked list
 *
 * Possible choices: DISABLED, CCLIB
 *
 * */
#define CCLIB_DS_ILIST  CC_FEAT_USE_CCLIB

/** Set, an unordered collection
 * 
 * This feature is inaccessible if CCLIB_ALLOC is DISABLED
 *
 * Possible choices: DISABLED, CCLIB, USER
 *
 * */
#define CCLIB_DS_SET    CC_FEAT_USE_CCLIB

/** Map, an unordered dictionary
 *
 * This feature is inaccessible if CCLIB_ALLOC is DISABLED
 *
 * Possible choices: DISABLED, CCLIB, USER
 *
 * */
#define CCLIB_DS_MAP    CC_FEAT_USE_CCLIB

/** Easy Going Configuration, the flat configuration file
 *
 * Possible choices: DISABLED, CCLIB
 *
 * */
#define CCLIB_EGCONF    CC_FEAT_USE_CCLIB

/** MiniDB, a simple database
 *
 * Possible choices: DISABLED, CCLIB, USER
 *
 * */
#define CCLIB_MDB       CC_FEAT_USE_CCLIB

/** CCLIB CmdParse, a command line parser
 *
 * Possible choices: DISABLED, CCLIB, USER
 *
 * */
#define CCLIB_CPARSE    CC_FEAT_USE_CCLIB

/** CCLIB Shell, a shell like scripting language
 *
 * This feature is inaccessible if CCLIB_ALLOC is DIABLED
 *
 * Possible choices: DISABLED, CCLIB
 *
 * */
#define CCLIB_SHELL     CC_FEAT_USE_SHELL

/** CCTest, a simple testing framework
 *
 * Possible choices: DISABLED, CCLIB
 *
 * */
#define CCLIB_TEST      CC_FEAT_USE_CCLIB

#endif /* CCLIB_CFG_TEMP_H */

