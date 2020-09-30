#ifndef CCLIB_CFG_TEMP_H
#define CCLIB_CFG_TEMP_H

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

/** Integer types library
 *
 * Possible choices: STD, USER
 *
 *   - If STD choosed, CCLIB will use stdint.h and re-export definitions
 *   - If USER choosed, CCLIB will include cc_inttypes.user.h
 *
 * */
#define CCLIB_INTLIB    CC_FEAT_USE_STD

/** Panic library
 *
 * Possible choices: DISABLED, STD, USER
 *
 *   - If DISABLED choosed, function cc_panic will still be generated,
 *     but with no effect
 *   - If STD choosed, cc_panic will call abort
 *   - If USER choosed, cc_panic will use implementation from
 *     cc_panic.user.c
 *
 * */
#define CCLIB_PANIC     CC_FEAT_USE_STD

/** Display library
 *
 * Possible choices: DISABLED, STD, USER
 *
 *   - If DISABLED choosed, display functions will still be generated,
 *     but with no effect
 *   - If STD choosed, display functions will call stdio functions
 *   - If USER choosed, display functions will use implementations
 *     from cc_disp.user.c
 *
 * */
#define CCLIB_DISP      CC_FEAT_USE_STD

/** Override default signed or unsigned size types of CCLIB.
 *
 * By default, CCLIB uses size_t for unsigned size type, ptrdiff_t
 * for unsigned size type.
 *
 * */
/* #define CCLIB_SIZE_TYPE size_t */
/* #define CCLIB_SSIZE_TYPE ptrdiff_t */

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

/** CCLIB CmdParse, a command line argument parser
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

