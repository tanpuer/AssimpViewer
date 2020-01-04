#include <iostream>
#include <fstream>

#ifdef WIN32
#include <Windows.h>
#else

#include <sys/time.h>

#endif

#include "utils.h"

long long GetCurrentTimeMillis() {
#ifdef WIN32
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif
}
