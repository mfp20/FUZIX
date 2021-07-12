#ifndef _RT_USB_PROTOCOL_DOT_H
#define _RT_USB_PROTOCOL_DOT_H

#include "rt.h"

#define USB_PACKET_CTRL_BIT 8
#define USB_PACKET_ID_CTRL 0
#define USB_PACKET_ID_FS1 1
#define USB_PACKET_ID_FS2 2
#define USB_PACKET_ID_FS3 4
#define USB_PACKET_ID_CORE1 8
#define USB_PACKET_ID_TTY1 16
#define USB_PACKET_ID_TTY2 32
#define USB_PACKET_ID_TTY3 64

typedef void (*usb_packet_control_fptr)(uint8_t len);
typedef uint32_t *(*usb_fs_buffer_addr_fptr)(uint8_t fs_id, bool ctrl, uint8_t len);
typedef void (*usb_fs_rx_fptr)(uint8_t fs_id, bool ctrl, uint8_t len);
typedef void (*usb_packet_chardev_fptr)(bool ctrl, uint8_t len);

extern uint8_t vend_expected;
extern uint8_t vend_packet[256];

void usb_packet_set_size(void);
bool usb_packet_ready(void);
void usb_packet_dispatch(uint8_t len);

extern usb_packet_control_fptr usb_packet_control_rx;
extern usb_fs_buffer_addr_fptr usb_fs_block_addr;
extern usb_fs_rx_fptr usb_fs_rx;
extern usb_packet_chardev_fptr usb_packet_core1_rx;
extern usb_packet_chardev_fptr usb_packet_tty1_rx;
extern usb_packet_chardev_fptr usb_packet_tty2_rx;
extern usb_packet_chardev_fptr usb_packet_tty3_rx;

#endif
