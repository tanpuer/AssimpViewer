//
// Created by templechen on 2019-09-04.
//

#include <sys/time.h>
#include "utils.h"

long javaTimeMillis() {
    timeval time;
    gettimeofday(&time, nullptr);
    return long(time.tv_sec) * 1000 + long(time.tv_usec / 1000);
}