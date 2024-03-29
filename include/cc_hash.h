#include <cc_defs.h>
typedef struct cc_hasher * cc_hasher_handle;
/**
 * Initialize a hasher (Platform dependent).
 * @param hasher pointer to the hasher to be initialized
 * @param seed seed to do the initialization
 */
CC_ATTRIBUTE_EXPORT
cc_hasher_handle cc_initialize_unstable_hasher(cc_uint64 seed);
/**
 * Derive the hash value according to the current state of hasher.
 * @param hasher pointer to the hasher
 * @return hash value
 */
CC_ATTRIBUTE_EXPORT
cc_uint64 cc_finalize_unstable_hasher(cc_hasher_handle hasher);
/**
 * Add more information into the hasher.
 * @param hasher pointer to the hasher
 * @param input pointer to the input buffer
 * @param length length of the buffer
 */
CC_ATTRIBUTE_EXPORT
void cc_unstable_hasher_digest(cc_hasher_handle hasher, const void * input, cc_size length);

/**
 * Initialize a hasher (Platform independent).
 * @param hasher pointer to the hasher to be initialized
 * @param seed seed to do the initialization
 */
CC_ATTRIBUTE_EXPORT
cc_hasher_handle cc_initialize_stable_hasher(cc_uint64 seed);
/**
 * Derive the hash value according to the current state of hasher.
 * @param hasher pointer to the hasher
 * @return hash value
 */
CC_ATTRIBUTE_EXPORT
cc_uint64 cc_finalize_stable_hasher(cc_hasher_handle hasher);
/**
 * Add more information into the hasher.
 * @param hasher pointer to the hasher
 * @param input pointer to the input buffer
 * @param length length of the buffer
 */
CC_ATTRIBUTE_EXPORT
void cc_stable_hasher_digest(cc_hasher_handle hasher, const void * input, cc_size length);

/**
 * Calculate platform dependent oneshot hash value.
 * @param input pointer to the input buffer
 * @param length length of the buffer
 * @param seed seed to initialize the underlying hasher
 * @return hash value
 */
CC_ATTRIBUTE_EXPORT
cc_uint64 cc_unstable_hash(const void * input, cc_size length, cc_uint64 seed);
/**
 * Calculate platform independent oneshot hash value.
 * @param input pointer to the input buffer
 * @param length length of the buffer
 * @param seed seed to initialize the underlying hasher
 * @return hash value
 */
CC_ATTRIBUTE_EXPORT
cc_uint64 cc_stable_hash(const void * input, cc_size length, cc_uint64 seed);

/**
 * Check if the platform-dependent hasher is accelerated by hardware AES.
 * @return checking result
 */
CC_ATTRIBUTE_EXPORT
_Bool cc_hash_simd_accelerated(void);
/**
 * Check if the platform-dependent hasher is accelerated by hardware AES with wider width.
 * @return checking result
 */
CC_ATTRIBUTE_EXPORT
_Bool cc_hash_wide_simd_accelerated(void);
