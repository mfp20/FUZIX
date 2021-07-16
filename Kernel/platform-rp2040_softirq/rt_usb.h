#ifndef _RT_USB_DOT_H
#define _RT_USB_DOT_H

#include "rt.h"
#include "rt_usb_mplex.h"

void usb_init(void);
void usb_cdc3_set_cb(byte_tx_t rx_cb);
void usb_vend1_set_cb(byte_tx_t rx_cb);
void usb_vend2_set_cb(byte_tx_t rx_cb);

uint8_t usb_cdc0_read(void);
void usb_cdc0_write(uint8_t b);
bool usb_cdc0_writable(void);

uint8_t usb_cdc1_read(void);
void usb_cdc1_write(uint8_t b);
bool usb_cdc1_writable(void);

uint8_t usb_cdc2_read(void);
void usb_cdc2_write(uint8_t b);
bool usb_cdc2_writable(void);

uint8_t usb_cdc3_read(void);
void usb_cdc3_write(uint8_t b);
bool usb_cdc3_writable(void);

#endif
