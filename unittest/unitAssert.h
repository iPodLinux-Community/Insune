#ifndef _UNITASSERT_H_
#define _UNITASSERT_H_
#include <stdio.h>

#define unitAssert(x) \
  do { \
    if (!(x)) { \
      return false; \
    } \
  } while (0)

#endif
