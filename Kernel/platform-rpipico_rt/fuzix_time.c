#include "platform.h"

//--------------------------------------------------------------------+
// real time clock
//--------------------------------------------------------------------+

/*
* Pico's structure for RTC

typedef struct {
    int16_t year;    
    int8_t month;    
    int8_t day;      
    int8_t dotw;     
    int8_t hour;     
    int8_t min;      
    int8_t sec;      
} datetime_t;

* Fuzix structure for RTC

struct cmos_rtc
{
        uint8_t type;
        union {
                uint8_t bytes[8];
                time_t clock;
        } data;
};
*/

uint_fast8_t platform_rtc_secs(void) {
    static uint8_t last;

    datetime_t pico_dt;
    if (rtc_get(&pico_dt))
        last = pico_dt.sec;

    return last;
}

// I'm using setdate as /dev/rtc consumer and it fails on
//                  (read(fd, &rtc, sizeof(rtc)) != sizeof(rtc)) {
// Looks like /dev/rtc returns 12 bytes but "struct cmos_rtc rtc" is 16 bytes.
// My best guess is about different time_t being used by kernel and setdate, resulting in a different struct cmos_rtc rtc size (because struct cmos_rtc rtc contains a time_t)
int platform_rtc_read(void) {
    /*
    // get datetime_t from pico's rtc
    datetime_t pico_dt;
    if (!rtc_get(&pico_dt))
        return -1;

    // check len
    uint16_t len = sizeof(struct cmos_rtc);
    if (udata.u_count < len)
            len = udata.u_count;

    // fill struct cmos_rtc using pico's datetime_t
    struct cmos_rtc cmos;
    cmos.type = CMOS_RTC_DEC; // byte encoded YYYY MM DD HH MM SS, little endian
    uint8_t *p = cmos.data.bytes;
    *p++ = pico_dt.year;
    *p++ = pico_dt.year >> 8;
    *p++ = pico_dt.month;
    *p++ = pico_dt.day;
    *p++ = pico_dt.hour;
    *p++ = pico_dt.min;
    *p++ = pico_dt.sec;
    *p++ = pico_dt.dotw;

    // write cmos_rtc to system
    if (uput(&cmos, udata.u_base, len) == -1)
        return -1;
    return len;
    */
}

int platform_rtc_write(void) {
    /* need debug
    uint16_t len = sizeof(struct cmos_rtc);
    struct cmos_rtc cmos;

    if (udata.u_count != len) {
        udata.u_error = EINVAL;
        return -1;
    }

    if (uget(&cmos, udata.u_base, len) == -1)
        return -1;

    if (!setdate_rtc((int16_t)(cmos.data.bytes[0] + (((int16_t)cmos.data.bytes[1])<<8)),
                    (int8_t)cmos.data.bytes[2],
                    (int8_t)cmos.data.bytes[3],
                    (int8_t)cmos.data.bytes[7],
                    (int8_t)cmos.data.bytes[4],
                    (int8_t)cmos.data.bytes[5],
                    (int8_t)cmos.data.bytes[6])) {
        udata.u_error = -EIO;
        return -1;
    }

    return len;
    */
}

//--------------------------------------------------------------------+
// system tick
//--------------------------------------------------------------------+

static repeating_timer_t ticker_timer;

static bool tick_trigger(repeating_timer_t *rt)
{
	if (fuzix_ready && queue_is_empty(&softirq_out_q))
	{
		timer_interrupt();
	}
	else
	{
		softirq_out(DEV_ID_TIMER, SIG_ID_TICK, 0, NULL);
	}

	return true;
}

void virtual_ticker_init(void) {
	alarm_pool_add_repeating_timer_us(alarm_pool[ALARM_POOL_TICK], (1000000 / TICKSPERSEC), tick_trigger, NULL, &ticker_timer);
}
