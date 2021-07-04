#ifndef _SDK_VIRTDEV_CORE1_DOT_H
#define _SDK_VIRTDEV_CORE1_DOT_H

#include "rt.h"
#include "rt_chardev.h"

typedef void (*core1_main_t)(void);

extern void core1_init(core1_main_t c1main, byte_tx_t rx_cb);
extern bool core1_readable(void);
extern bool core1_writable(void);
extern uint8_t core1_read(void);
extern void core1_write(uint8_t b);

#endif
