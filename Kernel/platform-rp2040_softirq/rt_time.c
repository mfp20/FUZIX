#include "rt_time.h"
#include "rt_log.h"

alarm_pool_t *alarm_pool[4];

// TODO
static void jitter_fix(int32_t jitter) {
    static int32_t sum = 0;

    sum += jitter;
    if ((sum>=1000)||(sum<=-1000)) {
        WARN("jitter %ld us, jitter_fix NOT IMPLEMENTED", sum);
        // TODO use rtc clock as a reference to re-offset systick
        sum = 0;
    }
}

static void jitter_eval(void) {
    static int32_t last = 0;
    uint32_t now = time_us_32();
    int32_t jitter = 0;

    if (now<last) {
        jitter = UINT32_MAX-last;
        jitter += now;
    }
    else
    {
        jitter = now-last;
    }
    jitter -= 60000000;
    jitter_fix(jitter);
    last = now;
}

void time_init(void) {
    // rtc setup
    rtc_init();
    datetime_t t = {
            .year  = 1970,
            .month = 01,
            .day   = 01,
            .dotw  = 04,
            .hour  = 00,
            .min   = 00,
            .sec   = 00
    };
    rtc_set_datetime(&t);
    // rtc alarm for jitter control
    rtc_enable_alarm();
    t.year  = -1;
    t.month = -1;
    t.day   = -1;
    t.dotw  = -1;
    t.hour  = -1;
    t.min   = -1;
    t.sec   = 00; // triggers every minute at 00 seconds
    rtc_set_alarm(&t, jitter_eval);

    // get pointers to all available alarm pools
    uint8_t missing = ALARM_POOL_BE;
    if (PICO_TIME_DEFAULT_ALARM_POOL_HARDWARE_ALARM_NUM==0) {
        alarm_pool_init_default();
        alarm_pool[ALARM_POOL_BE] = alarm_pool_get_default();
        missing = ALARM_POOL_PRIO;
    }
    else
        alarm_pool[ALARM_POOL_PRIO] = alarm_pool_create(0, 16);
    if (PICO_TIME_DEFAULT_ALARM_POOL_HARDWARE_ALARM_NUM==1) {
        alarm_pool_init_default();
        alarm_pool[ALARM_POOL_BE] = alarm_pool_get_default();
        missing = ALARM_POOL_TICK;
    }
    else
        alarm_pool[ALARM_POOL_TICK] = alarm_pool_create(1, 16);
    if (PICO_TIME_DEFAULT_ALARM_POOL_HARDWARE_ALARM_NUM==2) {
        alarm_pool_init_default();
        alarm_pool[ALARM_POOL_BE] = alarm_pool_get_default();
        missing = ALARM_POOL_LOW;
    }
    else
        alarm_pool[ALARM_POOL_LOW] = alarm_pool_create(2, 16);
    if (PICO_TIME_DEFAULT_ALARM_POOL_HARDWARE_ALARM_NUM==3) {
        alarm_pool_init_default();
        alarm_pool[ALARM_POOL_BE] = alarm_pool_get_default();
    }
    else
        alarm_pool[missing] = alarm_pool_create(3, 16);
}

bool rtc_datetime_get(datetime_t *t) {
    return rtc_get_datetime(t);
}

bool rtc_datetime_set(int16_t y, int8_t mo, int8_t d, int8_t dotw, int8_t h, int8_t mi, int8_t s) {
    datetime_t t = {
            .year  = y,
            .month = mo,
            .day   = d,
            .dotw  = dotw,
            .hour  = h,
            .min   = mi,
            .sec   = s
    };
    return rtc_set_datetime(&t);
}

bool (*usb_datetime_get)(datetime_t *t) = NULL;

uint32_t monotonic32(void) {
    return time_us_32();
}

uint64_t monotonic64(void) {
    return time_us_64();
}

/* vim: sw=4 ts=4 et: */
