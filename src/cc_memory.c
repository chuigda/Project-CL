#include <cc_cfg.h>
#include <cc_memory.h>
#include <cc_assert.h>

#include <cc_impl.h>

#ifdef PROJECT_CL_MEMORY_USE_LIBC
#   include <string.h>
void cc_memset(void *ptr, cc_uint8 value, cc_size cnt) {
    RT_CONTRACT(ptr)

    memset(ptr, (int)value, (size_t)cnt);
}

void cc_memcpy(void *dst, void *src, cc_size cnt) {
    RT_CONTRACT(dst && src)

    memcpy(dst, src, (size_t)cnt);
}

void cc_memmove(void *dst, void *src, cc_size cnt) {
    RT_CONTRACT(dst && src)

    memmove(dst, src, (size_t)cnt);
}
#else

#endif
