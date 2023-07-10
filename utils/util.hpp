#ifndef PCC_UTILS_UTIL_H
#define PCC_UTILS_UTIL_H


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


inline void error(char *fmt, ...) 
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}


#define unreachable() \
    error("internal error at %s:%d", __FILE__, __LINE__)


// Takes a printf-style format string and returns a formatted string.
inline char *format(char *fmt, ...) {
  char *buf;
  size_t buflen;
  FILE *out = open_memstream(&buf, &buflen);

  va_list ap;
  va_start(ap, fmt);
  vfprintf(out, fmt, ap);
  va_end(ap);
  fclose(out);
  return buf;
}


/**
 * @brief Round up \p n to the nearest multiple of \p align
 * 
 * @param n 
 * @param align 
 * @return int 
 */
inline int align_to(int n, int align)
{
    return (n + align - 1) / align * align;
}


#endif /* PCC_UTILS_UTIL_H */
