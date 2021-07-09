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

// stdio helpers
extern void stdio_putchar(uint8_t c);
extern void stdio_printf(const char *fmt, ...);

// log helpers
extern void log_set_level(uint8_t level);
extern void log_test_color(void);
extern void log_snprintf_hex(unsigned char *in, unsigned int count, char *out);
extern void log_stdio(uint8_t level, const char *fmt, ...);
#define EMERG(fmt, ...) log_stdio(LEVEL_EMERG, fmt, ##__VA_ARGS__)
#define ALERT(fmt, ...) log_stdio(LEVEL_ALERT, fmt, ##__VA_ARGS__)
#define CRIT(fmt, ...) log_stdio(LEVEL_CRIT, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...) log_stdio(LEVEL_ERR, fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) log_stdio(LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define NOTICE(fmt, ...) log_stdio(LEVEL_NOTICE, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) log_stdio(LEVEL_INFO, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) log_stdio(LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#endif
