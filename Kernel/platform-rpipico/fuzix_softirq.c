#include "platform.h"

// signal fuzix is in 'ei'/'irqrestore', ie: core code can be used freely
bool fuzix_ready = true;

uint32_t di(void) {
	fuzix_ready = false;
	return 0;
}

void ei(void) {
	fuzix_ready = true;
	devvirt_quick();
}

void irqrestore(uint32_t ps) {
	fuzix_ready = true;
	devvirt_quick();
}

/* vim: sw=4 ts=4 et: */
