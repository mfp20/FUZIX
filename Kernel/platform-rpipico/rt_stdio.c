#include "rt_stdio.h"
#include "rt_log.h"
#include "rt_softirq.h"

#include <stdio.h>
#include <stdarg.h>

void stdio_putchar(uint8_t c) {
    putchar(c);
}

void stdio_printf(const char *fmt, ...) {
    va_list arglist;
    va_start( arglist, fmt );
    vprintf(fmt, arglist);
    va_end( arglist );
}

void stdio_log(uint8_t level, const char *fmt, ...) {
    char buffer[strlen(fmt)*3];
    va_list arglist;
    va_start( arglist, fmt );
    vsnprintf(buffer, strlen(fmt)*3, fmt, arglist);
    va_end( arglist );

    switch (level) {
        case LEVEL_EMERG:
            LOG_EME("%s", buffer);
        break;
        case LEVEL_ALERT:
            LOG_ALE("%s", fmt);
        break;
        case LEVEL_CRIT:
            LOG_CRI("%s", fmt);
        break;
        case LEVEL_ERR:
            LOG_ERR("%s", fmt);
        break;
        case LEVEL_WARNING:
            LOG_WAR("%s", fmt);
        break;
        case LEVEL_NOTICE:
            LOG_NOT("%s", fmt);
        break;
        case LEVEL_INFO:
            LOG_INF("%s", buffer);
        break;
        case LEVEL_DEBUG:
            LOG_DEB("%s", fmt);
        break;
    }
}

/* vim: sw=4 ts=4 et: */
