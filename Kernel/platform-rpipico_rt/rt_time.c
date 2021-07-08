#include "rt_time.h"

uint32_t monotonic32(void) {
    return time_us_32();
}

uint64_t monotonic64(void) {
    return time_us_64();
}

/* vim: sw=4 ts=4 et: */
