#ifndef _SDK_VIRTDEV_UART_DOT_H
#define _SDK_VIRTDEV_UART_DOT_H

#include "rt.h"
#include "rt_chardev.h"

uint8_t uart0_read(void);
void uart0_write(uint8_t b);
bool uart0_writable(void);

uint8_t uart1_read(void);
void uart1_write(uint8_t b);
bool uart1_writable(void);

void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
void uart0_set_cb(byte_tx_t rx_cb);

void uart1_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
void uart1_set_cb(byte_tx_t rx_cb);

void uart_stdio(uint8_t id, bool stdio, bool test);

#endif
