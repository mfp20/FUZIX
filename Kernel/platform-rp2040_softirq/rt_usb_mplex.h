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

#define USB_CTRL_ID_SYN 0
#define USB_CTRL_ID_CONNECT 1
#define USB_CTRL_ID_DISCONNECT 2
#define USB_CTRL_ID_DATETIME 3
#define USB_CTRL_ID_BLOCKDEV_CONNECT 4
#define USB_CTRL_ID_BLOCKDEV_DISCONNECT 5
#define USB_CTRL_ID_CHARDEV_CONNECT 6
#define USB_CTRL_ID_CHARDEV_DISCONNECT 7
#define USB_CTRL_ID_REBOOT 255

#define USB_DISK_OP_LBA 0
#define USB_DISK_OP_READ 1
#define USB_DISK_OP_WRITE 2
#define USB_DISK_OP_TRIM 3

typedef void (*usb_packet_chardev_fptr)(bool ctrl, uint8_t len);

extern uint8_t vend_expected;

extern bool usb_vend0_connected;
extern bool usb_vend0_chardev_enabled;
extern bool usb_vend0_blockdev_enabled;

void usb_rx_packet_set_size(void);
void usb_rx_packet_dispatch(uint8_t len);

bool usb_connection_req(void);
bool usb_datetime_req(datetime_t *t);

uint32_t usb_disk_lba_req(uint8_t disk_id);
bool usb_disk_read_req(uint8_t disk_id, uint32_t lba);
bool usb_disk_write_req(uint8_t disk_id, uint32_t lba);
bool usb_disk_trim_req(uint8_t disk_id, uint32_t lba);

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
