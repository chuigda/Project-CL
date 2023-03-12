#include <cc_memory.h>

#ifdef PROJECT_CL_MEMORY_USE_LIBC
#   include <stdlib.h>
void cc_memset(void *ptr, cc_uint8 value, cc_size cnt) {
#   ifdef PROJECT_CL_RUNTIME_CHECK
    cc_assert(ptr);
    if (!ptr) {
        return;
    }
#   endif
    memset(ptr, (int)value, (size_t)cnt);
}

void cc_memcpy(void *dst, void *src, cc_size cnt) {
#   ifdef PROJECT_CL_RUNTIME_CHECK
    cc_assert(dst && src);
    if (!(dst && src)) {
        return;
    }
#   endif
    memcpy(dst, src, (size_t)cnt);
}

void cc_memmove(void *dst, void *src, cc_size cnt) {
#   ifdef PROJECT_CL_RUNTIME_CHECK
    cc_assert(dst && src);
    if (!(dst && src)) {
        return;
    }
#   endif
    memmove(dst, src, (size_t)cnt);
}
#else

#endif
