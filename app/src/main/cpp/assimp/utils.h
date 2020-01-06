#ifndef OGLDEV_UTIL_H
#define    OGLDEV_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>

using namespace std;

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#define RANDOM random
#define SRANDOM srandom(getpid())

long long GetCurrentTimeMillis();

#endif    /* OGLDEV_UTIL_H */

