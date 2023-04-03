#ifndef PROJECT_CL_ENCODING_H
#define PROJECT_CL_ENCODING_H

#include <cc_defs.h>

#ifndef PROJECT_CL_BUILD_ENCODING
#   error PROJECT_CL_BUILD_ENCODING not defined
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @o_len: Pointer to output length variable, or @c NULL if not used
 * @returns Allocated string of o_len + 1 bytes of encoded data,
 * or @c NULL on failure
 */
cc_char *cc_base64_encode(const void *src, const cc_size len, cc_size *o_len);

/**
 * @brief Base64 decode
 * @data: Data to be decoded
 * @len: Length of the data to be decoded
 * @o_len: Pointer to output length variable, or @c NULL if not used
 * @returns Allocated buffer of decoded data,
 * or @c NULL on failure
 */
cc_uchar *cc_base64_decode(const cc_char *data, const cc_size len, cc_size *o_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_ENCODING_H */
