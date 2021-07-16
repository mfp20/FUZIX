#ifndef _RT_CHARDEV_DOT_H
#define _RT_CHARDEV_DOT_H

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
extern uint8_t tty_cd[4];

extern uint8_t stdio_byte;
extern uint8_t tty1_byte;
extern uint8_t tty2_byte;
extern uint8_t tty3_byte;

uint8_t chardev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w);
void chardev_mod(uint8_t chardev_id, byte_rx_t r, byte_tx_t t, byte_ready_t w);

uint8_t fuzix_select_read(void);
void fuzix_select_write(uint8_t b);
bool fuzix_select_writable(void);

uint8_t rt_select_read(void);
void rt_select_write(uint8_t b);
bool rt_select_writable(void);

uint8_t tty3_select_read(void);
void tty3_select_write(uint8_t b);
bool tty3_select_writable(void);

#endif
