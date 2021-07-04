#ifndef _SDK_VIRTDEV_UART_DOT_H
#define _SDK_VIRTDEV_UART_DOT_H

#include "rt.h"

extern void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t uart0_read(void);
extern void uart0_write(uint8_t b);
extern bool uart0_writable(void);

extern void uart1_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t uart1_read(void);
extern void uart1_write(uint8_t b);
extern bool uart1_writable(void);

extern void uart_stdio(uint8_t id, bool stdio);

#endif
