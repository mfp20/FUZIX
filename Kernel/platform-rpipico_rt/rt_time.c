#include "rt_time.h"
#include "rt_log.h"

alarm_pool_t *alarm_pool[4];
static uint32_t last = 0;

static void jitter_eval(void) {
    uint32_t now = time_us_32();
    if (now<last) {
        uint32_t val = UINT32_MAX-last;
        val += now;
        //INFO("jitter %ld us", val-1000000);
    }
    else
        ; //INFO("jitter %ld us", (now-last)-1000000);
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

    alarm_pool[0] = alarm_pool_create(0, 16);
    alarm_pool[1] = alarm_pool_create(1, 16);
    alarm_pool[2] = alarm_pool_create(2, 16);
    alarm_pool_init_default();
    alarm_pool[3] = alarm_pool_get_default();
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
