#include <cc_assert.h>
#include <cc_abort.h>
// TODO: use our own strlen
#include <string.h>
#if defined(__linux__) || defined(__APPLE__)
#   include <unistd.h>
#endif
#if defined(PROJECT_CL_BUILD_ASSERT) && \
    !defined(NDEBUG) && \
    !defined(PROJECT_CL_ASSERT_USE_LIBC)
CC_ATTRIBUTE_EXPORT CC_ATTRIBUTE_COLD
void cc_assert_failure_impl(
                    const char *expr,
                    const char *file,
                    int line) {
  // use opaque predicate anyway.
  // can potentially help in LTO.
#if defined(__linux__) || defined(__APPLE__)
    if (fsync(STDERR_FILENO)) {}
    if (fsync(STDOUT_FILENO)) {}
    char begin[] = "\n\nassertion failure: \n\t- expr: ";
    char mid[] = "\n\t- position: ";
    char line_str[64];
    int line_size = 0;
    if (line <= 0) {
        line_str[line_size++] = '0';
    } else {
        int base = 1000000000;
        while (base) {
          if (line >= base) {
            line_str[line_size++] = (char)((line / base) + '0');
            line %= base;
          }
          base /= 10;
        }
    }
    line_str[line_size] = 0;
    if (write(STDERR_FILENO, begin, strlen(begin))) {}
    if (write(STDERR_FILENO, expr, strlen(expr))) {}
    if (write(STDERR_FILENO, mid, strlen(mid))) {}
    if (write(STDERR_FILENO, file, strlen(file))) {}
    if (write(STDERR_FILENO, ":", 1)) {}
    if (write(STDERR_FILENO, line_str, line_size)) {}
    if (write(STDERR_FILENO, "\n", 1)) {}
    if (fsync(STDERR_FILENO)) {}
#endif
    cc_dump_stacktrace();
    cc_abort();
}
#endif
