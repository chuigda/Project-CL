#ifdef _MSC_VER
#include <intrin.h>
#endif
void cc_blackhole_(char volatile *ptr) {
    (void) ptr;
    // forbid LTO to optimize away the pointer
#ifdef __GNUC__
#if defined(__clang__)
    __asm__ volatile("" : "+r,m"(*ptr) : : "memory");
#else
    __asm__ volatile("" : "+m,r"(*ptr) : : "memory");
#endif
#endif
#ifdef _MSC_VER
    _ReadWriteBarrier();
#endif
}
