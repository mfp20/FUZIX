#ifndef _SDK_VIRTDEV_UART_DOT_H
#define _SDK_VIRTDEV_UART_DOT_H

#include "rt.h"
#include "rt_chardev.h"

void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
void uart0_set_cb(byte_tx_t rx_cb);

#endif
