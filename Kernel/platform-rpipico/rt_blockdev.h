#ifndef _SDK_BLOCKDEV_DOT_H
#define _SDK_BLOCKDEV_DOT_H

#include "rt.h"

typedef uint8_t (*block_rx_t)(void);
typedef void (*block_tx_t)(uint8_t);
typedef bool (*block_ready_t)(void);
typedef struct blockdev_s
{
	block_rx_t rx;
	block_tx_t tx;
	block_ready_t ready;
} blockdev_t;

extern blockdev_t *blockdev;

uint8_t blockdev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w);
void blockdev_mod(uint8_t n, byte_rx_t r, byte_tx_t t, byte_ready_t w);

#endif
