#ifndef OGLDEV_UTIL_H
#define    OGLDEV_UTIL_H

#ifndef WIN32

#include <unistd.h>

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>

using namespace std;

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#ifdef WIN32
#define SNPRINTF _snprintf_s
#define VSNPRINTF vsnprintf_s
#define RANDOM rand
#define SRANDOM srand((unsigned)time(NULL))
#else
#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#define RANDOM random
#define SRANDOM srandom(getpid())
#endif

long long GetCurrentTimeMillis();

#endif    /* OGLDEV_UTIL_H */

