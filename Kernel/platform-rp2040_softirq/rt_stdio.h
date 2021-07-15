#ifndef _SDK_STDIO_DOT_H
#define _SDK_STDIO_DOT_H

#include "config.h"
#include "rt.h"

extern char stdio_buffer[256];
extern stdio_driver_t stdio_driver;

extern void stdio_putchar(uint8_t b);
extern void stdio_kputchar(uint8_t b);
extern void stdio_printf(const char *fmt, ...);

extern void log_set_level(uint8_t level);
extern void log_snprintf_hex(unsigned char *in, unsigned int count, char *out);
extern void log_test_color(void);

#endif
