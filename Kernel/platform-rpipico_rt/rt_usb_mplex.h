#ifndef _RT_USB_PROTOCOL_DOT_H
#define _RT_USB_PROTOCOL_DOT_H

#include "rt.h"

#define USB_PACKET_CTRL_BIT 8
#define USB_PACKET_ID_CTRL 0
#define USB_PACKET_ID_DISK1 1
#define USB_PACKET_ID_DISK2 2
#define USB_PACKET_ID_DISK3 4
#define USB_PACKET_ID_CORE1 8
#define USB_PACKET_ID_TTY1 16
#define USB_PACKET_ID_TTY2 32
#define USB_PACKET_ID_TTY3 64

#define USB_CTRL_ID_CONNECT 0
#define USB_CTRL_ID_DISCONNECT 1
#define USB_CTRL_ID_BLOCKDEV_CONNECT 2
#define USB_CTRL_ID_BLOCKDEV_DISCONNECT 3
#define USB_CTRL_ID_CHARDEV_CONNECT 4
#define USB_CTRL_ID_CHARDEV_DISCONNECT 5
#define USB_CTRL_ID_REBOOT 255

typedef uint32_t *(*usb_disk_buffer_addr_fptr)(uint8_t disk_id, bool ctrl, uint8_t len);
typedef void (*usb_disk_rx_fptr)(uint8_t disk_id, bool ctrl, uint8_t len);
typedef void (*usb_packet_chardev_fptr)(bool ctrl, uint8_t len);

extern uint8_t vend_expected;
extern uint8_t vend_packet[256];

extern bool usb_vend0_chardev_connected;
extern bool usb_vend0_blockdev_connected;

extern usb_packet_chardev_fptr usb_packet_rx_core1;
extern usb_packet_chardev_fptr usb_packet_rx_tty1;
extern usb_packet_chardev_fptr usb_packet_rx_tty2;
extern usb_packet_chardev_fptr usb_packet_rx_tty3;

void usb_rx_packet_set_size(void);
void usb_rx_packet_dispatch(uint8_t len);

bool usb_timestamp_req(datetime_t *t);
uint32_t usb_disk_lba_req(uint8_t disk_id);

void usb_vend0_set_cb(usb_packet_chardev_fptr rx_packet_core1_cb,
						usb_packet_chardev_fptr rx_packet_tty1_cb,
						usb_packet_chardev_fptr rx_packet_tty2_cb,
						usb_packet_chardev_fptr rx_packet_tty3_cb
						);

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
