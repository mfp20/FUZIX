#include "rt_stdio.h"

#include <stdio.h>
#include <stdarg.h>

void stdio_putchar(uint8_t c) {
	putchar(c);
}

void stdio_printf(const char *fmt, ...) {
    va_list arglist;
    va_start( arglist, fmt );
    vprintf( fmt, arglist );
    va_end( arglist );
}

/* vim: sw=4 ts=4 et: */
