#include "rt_stdio.h"

#include <stdio.h>

void stdio_putchar(uint8_t c) {
	putchar(c);
}

void stdio_printf(const char *fmt, ...) {
	printf(fmt);
}

/* vim: sw=4 ts=4 et: */
