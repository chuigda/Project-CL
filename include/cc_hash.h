#include <cc_defs.h>
typedef struct {
    _Alignas(16) cc_uint64 opaque[6];
} cc_hasher;

/**
 * Initialize a hasher (Platform dependent).
 * @param hasher pointer to the hasher to be initialized
 * @param seed seed to do the initialization
 */
void cc_initialize_hasher(cc_hasher * hasher, cc_uint64 seed);
/**
 * Derive the hash value according to the current state of hasher.
 * @param hasher pointer to the hasher
 * @return hash value
 */
cc_uint64 cc_finalize_hasher(const cc_hasher * hasher);
/**
 * Add more information into the hasher.
 * @param hasher pointer to the hasher
 * @param input pointer to the input buffer
 * @param length length of the buffer
 */
void cc_hasher_digest(cc_hasher * hasher, const void * input, cc_size length);
/**
 * Calculate platform dependent oneshot hash value.
 * @param input pointer to the input buffer
 * @param length length of the buffer
 * @param seed seed to initialize the underlying hasher
 * @return hash value
 */
cc_uint64 cc_unstable_hash(const void * input, cc_size length, cc_uint64 seed);
/**
 * Calculate platform independent oneshot hash value.
 * @param input pointer to the input buffer
 * @param length length of the buffer
 * @param seed seed to initialize the underlying hasher
 * @return hash value
 */
cc_uint64 cc_stable_hash(const void * input, cc_size length, cc_uint64 seed);
/**
 * Check if the platform-dependent hasher is accelerated by hardware AES.
 * @return checking result
 */
_Bool cc_hash_simd_accelerated(void);
/**
 * Check if the platform-dependent hasher is accelerated by hardware AES with wider width.
 * @return checking result
 */
_Bool cc_hash_wide_simd_accelerated(void);
