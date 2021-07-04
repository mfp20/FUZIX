#ifndef _SDK_CHARDEV_DOT_H
#define _SDK_CHARDEV_DOT_H

#include "rt.h"

typedef uint8_t (*byte_rx_t)(void);
typedef void (*byte_tx_t)(uint8_t);
typedef bool (*byte_ready_t)(void);
typedef struct chardev_s
{
	byte_rx_t rx;
	byte_tx_t tx;
	byte_ready_t ready;
} chardev_t;

extern chardev_t *chardev;

uint8_t chardev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w);
void chardev_mod(uint8_t chardev_id, byte_rx_t r, byte_tx_t t, byte_ready_t w);

#endif
