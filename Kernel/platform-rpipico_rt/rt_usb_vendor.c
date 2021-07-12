#include "rt_log.h"
#include "rt_usb_vendor.h"

#include <tusb.h>

uint8_t vend_expected = 0;
uint8_t vend_packet[256];

usb_packet_control_fptr usb_packet_control_rx = NULL;
usb_fs_buffer_addr_fptr usb_fs_block_addr = NULL;
usb_fs_rx_fptr usb_fs_rx = NULL;
usb_packet_chardev_fptr usb_packet_core1_rx = NULL;
usb_packet_chardev_fptr usb_packet_tty1_rx = NULL;
usb_packet_chardev_fptr usb_packet_tty2_rx = NULL;
usb_packet_chardev_fptr usb_packet_tty3_rx = NULL;

void usb_packet_set_size(void) {
	tud_vendor_n_read(0, &vend_expected, 1);
}

bool usb_packet_ready(void) {
	if (vend_expected)
		return true;
	else
		return false;
}

void usb_packet_dispatch(uint8_t len) {
    uint8_t select;
	tud_vendor_n_read(0, (void *)&select, 1);
    len--;
	bool ctrl = (select >> USB_PACKET_CTRL_BIT) & 1U;
	switch (select &= ~(1UL << USB_PACKET_CTRL_BIT)) {
		case USB_PACKET_ID_CTRL:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_control_rx)
                usb_packet_control_rx(len);
            else
                WARN("USB VEND0: control packet received but control callback is not set");
		break;
		case USB_PACKET_ID_FS1:
            if (usb_fs_block_addr&&usb_fs_rx) {
                void *addr = usb_fs_block_addr(1, ctrl, len);
                tud_vendor_n_read(0, addr, len);
                usb_fs_rx(1, ctrl, len);
            }
            else
            {
                WARN("USB VEND0: fs1 packet received but fs1 callbacks are not set");
                tud_vendor_n_read(0, (void *)&vend_packet, len);
            }
		break;
		case USB_PACKET_ID_FS2:
            if (usb_fs_block_addr&&usb_fs_rx) {
                void *addr = usb_fs_block_addr(2, ctrl, len);
                tud_vendor_n_read(0, addr, len);
                usb_fs_rx(2, ctrl, len);
            }
            else
            {
                WARN("USB VEND0: fs2 packet received but fs2 callbacks are not set");
                tud_vendor_n_read(0, (void *)&vend_packet, len);
            }
		break;
		case USB_PACKET_ID_FS3:
            if (usb_fs_block_addr&&usb_fs_rx) {
                void *addr = usb_fs_block_addr(3, ctrl, len);
                tud_vendor_n_read(0, addr, len);
                usb_fs_rx(3, ctrl, len);
            }
            else
            {
                WARN("USB VEND0: fs3 packet received but fs3 callbacks are not set");
                tud_vendor_n_read(0, (void *)&vend_packet, len);
            }
		break;
		case USB_PACKET_ID_CORE1:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_core1_rx)
                usb_packet_core1_rx(ctrl, len);
            else
                WARN("USB VEND0: core1 packet received but core1 callback is not set");
		break;
		case USB_PACKET_ID_TTY1:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_tty1_rx)
                usb_packet_tty1_rx(ctrl, len);
            else
                WARN("USB VEND0: tty1 packet received but tty1 callback is not set");
		break;
		case USB_PACKET_ID_TTY2:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_tty2_rx)
                usb_packet_tty2_rx(ctrl, len);
            else
                WARN("USB VEND0: tty2 packet received but tty2 callback isnot set");
		break;
		case USB_PACKET_ID_TTY3:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_tty3_rx)
                usb_packet_tty3_rx(ctrl, len);
            else
                WARN("USB VEND0: tty3 packet received but tty3 callback is not set");
		break;
		default:
            ERR("USB VEND0: Unknown packet");
		break;
	}
}
