#ifndef _SDK_STDIO_DOT_H
#define _SDK_STDIO_DOT_H

#include "config.h"
#include "rt.h"

// log levels the same as syslog
#define LEVEL_EMERG (0)
#define LEVEL_ALERT (1)
#define LEVEL_CRIT (2)
#define LEVEL_ERR (3)
#define LEVEL_WARNING (4)
#define LEVEL_NOTICE (5)
#define LEVEL_INFO (6)
#define LEVEL_DEBUG (7)

// log
extern void log_set_level(uint8_t level);
extern void log_test_color(void);
extern void log_snprintf_hex(unsigned char *in, unsigned int count, char *out);

// stdio helpers
extern void stdio_putchar(uint8_t c);
extern void stdio_printf(const char *fmt, ...);

extern void stdio_log(uint8_t level, const char *fmt, ...);

#define EMERG(fmt, ...) stdio_log(LEVEL_EMERG, fmt, ##__VA_ARGS__)
#define ALERT(fmt, ...) stdio_log(LEVEL_ALERT, fmt, ##__VA_ARGS__)
#define CRIT(fmt, ...) stdio_log(LEVEL_CRIT, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...) stdio_log(LEVEL_ERR, fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) stdio_log(LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define NOTICE(fmt, ...) stdio_log(LEVEL_NOTICE, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) stdio_log(LEVEL_INFO, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) stdio_log(LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#endif
