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

extern bool stdio_irq_done;
extern bool tty1_irq_done;
extern bool tty2_irq_done;
extern bool tty3_irq_done;
extern bool tty4_irq_done;

extern uint8_t stdio_byte;
extern uint8_t tty1_byte;
extern uint8_t tty2_byte;
extern uint8_t tty3_byte;
extern uint8_t tty4_byte;

uint8_t chardev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w);
void chardev_mod(uint8_t chardev_id, byte_rx_t r, byte_tx_t t, byte_ready_t w);

uint8_t stdio_select_read(void);
void stdio_select_write(uint8_t b);
bool stdio_select_writable(void);

uint8_t tty1_select_read(void);
void tty1_select_write(uint8_t b);
bool tty1_select_writable(void);

uint8_t tty2_select_read(void);
void tty2_select_write(uint8_t b);
bool tty2_select_writable(void);

#endif
