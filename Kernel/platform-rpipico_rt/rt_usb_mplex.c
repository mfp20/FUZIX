#include "rt_log.h"
#include "rt_usb_mplex.h"

#include <tusb.h>

uint8_t vend_expected = 0;
uint8_t vend_packet[256];
bool usb_vend_chardev_connected = false;

//--------------------------------------------------------------------+
// low level
//--------------------------------------------------------------------+

static void usb_packet_control_rx(uint8_t len) {
    switch ((uint8_t)vend_packet[0]) {
        case USB_CTRL_ID_CONNECT:
            // TODO what is vendor class "mounted"?
            WARN("USB VEND0: USB_CTRL_ID_CONNECT NOT IMPLEMENTED");
        break;
        case USB_CTRL_ID_DISCONNECT:
            // TODO what is vendor class "mounted"?
            WARN("USB VEND0: USB_CTRL_ID_DISCONNECT command NOT IMPLEMENTED");
        break;
        case USB_CTRL_ID_BLOCKDEV_CONNECT:
            WARN("USB VEND0: USB_CTRL_ID_BLOCKDEV_CONNECT command NOT IMPLEMENTED");
        break;
        case USB_CTRL_ID_BLOCKDEV_DISCONNECT:
            WARN("USB VEND0: USB_CTRL_ID_BLOCKDEV_DISCONNECT command NOT IMPLEMENTED");
        break;
        case USB_CTRL_ID_CHARDEV_CONNECT:
            //usb_vend_chardev_connected = true;
            WARN("USB VEND0: USB_CTRL_ID_CHARDEV_CONNECT command NOT IMPLEMENTED");
        break;
        case USB_CTRL_ID_CHARDEV_DISCONNECT:
            //usb_vend_chardev_connected = false;
            WARN("USB VEND0: USB_CTRL_ID_CHARDEV_DISCONNECT command NOT IMPLEMENTED");
        break;
        case USB_CTRL_ID_REBOOT:
            // TODO fuzix clean reboot?
            WARN("USB VEND0: USB_CTRL_ID_REBOOT NOT IMPLEMENTED");
        break;
        default:
            ERR("USB VEND0: Unknown ctrl command (%d)", (uint8_t)vend_packet[0]);
        break;
    }
}

uint32_t usb_req_lba(uint8_t disk_id) {

	return 0;
}

static uint32_t *usb_disk_block_addr(uint8_t disk_id, bool ctrl, uint8_t len) {

}

static void usb_disk_rx(uint8_t disk_id, bool ctrl, uint8_t len) {

}

usb_packet_chardev_fptr usb_packet_core1_rx = NULL;
usb_packet_chardev_fptr usb_packet_tty1_rx = NULL;
usb_packet_chardev_fptr usb_packet_tty2_rx = NULL;
usb_packet_chardev_fptr usb_packet_tty3_rx = NULL;

//--------------------------------------------------------------------+
// 
//--------------------------------------------------------------------+

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
    void *addr;

	switch (select &= ~(1UL << USB_PACKET_CTRL_BIT)) {
		case USB_PACKET_ID_CTRL:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            usb_packet_control_rx(len);
		break;
		case USB_PACKET_ID_DISK1:
            addr = usb_disk_block_addr(1, ctrl, len);
            tud_vendor_n_read(0, addr, len);
            usb_disk_rx(1, ctrl, len);
		break;
		case USB_PACKET_ID_DISK2:
            addr = usb_disk_block_addr(2, ctrl, len);
            tud_vendor_n_read(0, addr, len);
            usb_disk_rx(2, ctrl, len);
		break;
		case USB_PACKET_ID_DISK3:
            addr = usb_disk_block_addr(3, ctrl, len);
            tud_vendor_n_read(0, addr, len);
            usb_disk_rx(3, ctrl, len);
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
