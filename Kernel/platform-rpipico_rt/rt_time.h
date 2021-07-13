#ifndef _SDK_TIME_DOT_H
#define _SDK_TIME_DOT_H

#include "rt.h"

#define ALARM_POOL_PRIO 0
#define ALARM_POOL_TICK 1
#define ALARM_POOL_LOW  2
#define ALARM_POOL_BE   3

extern alarm_pool_t *alarm_pool[4];

void time_init(void);

// RTC
bool getdate_rtc(datetime_t *t);
bool setdate_rtc(int16_t y, int8_t mo, int8_t d, int8_t dotw, int8_t h, int8_t mi, int8_t s);

// systick
uint32_t monotonic32(void);
uint64_t monotonic64(void);

#endif
