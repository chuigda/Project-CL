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
* base64_encode - Base64 encode
* @src: Data to be encoded
* @len: Length of the data to be encoded
* @out_len: Pointer to output length variable, or %NULL if not used
* Returns: Allocated buffer of out_len bytes of encoded data,
* or empty string on failure
*/
cc_char *cc_base64_encode(const void *src, const cc_size len, cc_size *o_len);

cc_uchar *cc_base64_decode(const cc_char *data, const cc_size len, cc_size *o_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_ENCODING_H */
