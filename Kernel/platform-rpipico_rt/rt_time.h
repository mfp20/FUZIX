#ifndef _SDK_TIME_DOT_H
#define _SDK_TIME_DOT_H

#include "rt.h"

// NOTE: the rule of thumb to use the pools is "the more often, the lower priority"
#define ALARM_POOL_PRIO 0 // high priority alarm pool
#define ALARM_POOL_TICK 1 // system tick alarm pool
#define ALARM_POOL_LOW  2 // low priority alarm pool
#define ALARM_POOL_BE   3 // best effort alarm pool

extern alarm_pool_t *alarm_pool[4];

//
void time_init(void);

// RTC
bool rtc_get(datetime_t *t);
bool rtc_set(int16_t y, int8_t mo, int8_t d, int8_t dotw, int8_t h, int8_t mi, int8_t s);

// USB time
extern bool (*usb_get)(datetime_t *t);

// systick
uint32_t monotonic32(void); // returns lower 32 bits of monotonic64()
uint64_t monotonic64(void); // returns microseconds since boot

#endif
