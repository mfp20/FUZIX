
#include "platform.h"

bool devcore1_is_readable(void) {
	return multicore_fifo_rvalid();
}

bool devcore1_is_writable(void) {
	return multicore_fifo_wready();
}

uint8_t devcore1_getc_blocking(void) {
	return multicore_fifo_pop_blocking();
}

void devcore1_putc_blocking(uint8_t b) {
	multicore_fifo_push_blocking(b);
}

void devcore1_init(core1_main_t c1main) {
	multicore_launch_core1(c1main);
}

