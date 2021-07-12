#ifndef _RT_USB_DOT_H
#define _RT_USB_DOT_H

#include "rt.h"
#include "rt_usb_vendor.h"

void usb_init(void);
void usb_cdc0_set_cb(byte_tx_t rx_cb);
void usb_cdc1_set_cb(byte_tx_t rx_cb);
void usb_cdc2_set_cb(byte_tx_t rx_cb);
void usb_cdc3_set_cb(byte_tx_t rx_cb);
void usb_vend0_set_cb(usb_packet_control_fptr packet_control,
						usb_disk_buffer_addr_fptr disk_block_addr,
						usb_disk_rx_fptr disk_rx,
						usb_packet_chardev_fptr packet_core1,
						usb_packet_chardev_fptr packet_tty1,
						usb_packet_chardev_fptr packet_tty2,
						usb_packet_chardev_fptr packet_tty3
						);
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

uint8_t usb_vend_tty1_read(void);
void usb_vend_tty1_write(uint8_t b);
bool usb_vend_tty1_writable(void);

uint8_t usb_vend_tty2_read(void);
void usb_vend_tty2_write(uint8_t b);
bool usb_vend_tty2_writable(void);

uint8_t usb_vend_tty3_read(void);
void usb_vend_tty3_write(uint8_t b);
bool usb_vend_tty3_writable(void);

#endif
