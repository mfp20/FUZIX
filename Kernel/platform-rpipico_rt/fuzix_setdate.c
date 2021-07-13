#include "platform.h"

int setdate(int y, int mo, int d, int h, int mi, int s, uint8_t utc)
{
    struct tm *tm;
    time_t t;
    
    // get system time
    time(&t);
    // convert to tm struct
    tm = localtime(&t);
    // set new values
    tm->tm_year = y - 1900;
    tm->tm_mon = mo - 1;
    tm->tm_mday = d;
    tm->tm_hour = h;
    tm->tm_min = mi;
    tm->tm_sec = s;
    tm->tm_isdst = utc ? 0 : -1;
    // convert back to time_t
    t = mktime(tm);
    if (t == (time_t) -1) {
        //ERR("mktime: internal error.\n");
        return 127;
    }
    // set system time using new values
    if (stime(&t)) {
        //ERR("stime");
        return 2;
    }
    return 0;
}
