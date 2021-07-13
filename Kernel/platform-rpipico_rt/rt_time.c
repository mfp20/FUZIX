#include "rt_time.h"
#include "rt_log.h"

alarm_pool_t *alarm_pool[4];
uint32_t jitter = 0;

// TODO
static void jitter_fix(void) {
}

// TODO
static void jitter_eval(void) {
    static uint32_t last;

    // evaluate jitter
    uint32_t now = time_us_32();
    if (now<last) {
        jitter = UINT32_MAX-last;
        jitter += now;
        //INFO("jitter %ld us", val-1000000);
    }
    else
    {
        jitter = (now-last)-1000000;
        //INFO("jitter %ld us", );
    }

    //
    jitter_fix();

    //
    last = time_us_32();
}

void time_init(void) {
    rtc_init();
    // repeat every second, for ever
    datetime_t t = {
            .year  = -1,
            .month = -1,
            .day   = -1,
            .dotw  = -1,
            .hour  = -1,
            .min   = -1,
            .sec   = -1
    };    
    //rtc_set_alarm(&t, jitter_eval);

    // get pointers to all available alarm pools
    uint8_t missing = 3;
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

uint32_t monotonic32(void) {
    return time_us_32();
}

uint64_t monotonic64(void) {
    return time_us_64();
}

bool getdate_rtc(datetime_t *t) {
    return rtc_get_datetime(t);
}

bool setdate_rtc(int16_t y, int8_t mo, int8_t d, int8_t dotw, int8_t h, int8_t mi, int8_t s) {
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

/* vim: sw=4 ts=4 et: */
