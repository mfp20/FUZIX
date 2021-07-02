
#include "picosdk.h"

uint32_t monotonic32(void) {
    return time_us_32();
}

uint64_t monotonic64(void) {
    return time_us_64();
}
