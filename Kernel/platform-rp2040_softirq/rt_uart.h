#ifndef _RT_UART_DOT_H
#define _RT_UART_DOT_H

#include "rt.h"
#include "rt_chardev.h"

void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate);
void uart0_set_cb(byte_tx_t rx_cb);

#endif
