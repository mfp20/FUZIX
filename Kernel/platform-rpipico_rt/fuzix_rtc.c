#include "platform.h"

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

    // get datetime_t from pico's rtc
    datetime_t pico_dt;
    if (getdate_rtc(&pico_dt))
        last = pico_dt.sec;

    return last;
}

int platform_rtc_read(void) {
    // get datetime_t from pico's rtc
    datetime_t pico_dt;
    if (!getdate_rtc(&pico_dt))
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

    // write cmos_rtc to system
    if (uput(&cmos, udata.u_base, len) == -1)
        return -1;
    return len;
}

int platform_rtc_write(void) {
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
}
