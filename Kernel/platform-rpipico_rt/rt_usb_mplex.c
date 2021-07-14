#include "rt_log.h"
#include "rt_usb_mplex.h"

#include <tusb.h>

//--------------------------------------------------------------------+
// rx
//--------------------------------------------------------------------+

uint8_t vend_expected = 0;
uint8_t vend_packet[256];

static void usb_packet_rx_control(uint8_t len) {
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
            //usb_vend0_chardev_connected = true;
            WARN("USB VEND0: USB_CTRL_ID_CHARDEV_CONNECT command NOT IMPLEMENTED");
        break;
        case USB_CTRL_ID_CHARDEV_DISCONNECT:
            //usb_vend0_chardev_connected = false;
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

static uint32_t *usb_disk_block_addr(uint8_t disk_id, bool ctrl, uint8_t len) {

}

static void usb_packet_rx_disk_block(uint8_t disk_id, bool ctrl, uint8_t len) {

}

void usb_rx_packet_set_size(void) {
	tud_vendor_n_read(0, &vend_expected, 1);
}

void usb_rx_packet_dispatch(uint8_t len) {
    uint8_t select;
	tud_vendor_n_read(0, (void *)&select, 1);
    len--;
	bool ctrl = (select >> USB_PACKET_CTRL_BIT) & 1U;
    void *addr;

	switch (select &= ~(1UL << USB_PACKET_CTRL_BIT)) {
		case USB_PACKET_ID_CTRL:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            usb_packet_rx_control(len);
		break;
		case USB_PACKET_ID_DISK1:
            addr = usb_disk_block_addr(1, ctrl, len);
            tud_vendor_n_read(0, addr, len);
            usb_packet_rx_disk_block(1, ctrl, len);
		break;
		case USB_PACKET_ID_DISK2:
            addr = usb_disk_block_addr(2, ctrl, len);
            tud_vendor_n_read(0, addr, len);
            usb_packet_rx_disk_block(2, ctrl, len);
		break;
		case USB_PACKET_ID_DISK3:
            addr = usb_disk_block_addr(3, ctrl, len);
            tud_vendor_n_read(0, addr, len);
            usb_packet_rx_disk_block(3, ctrl, len);
		break;
		case USB_PACKET_ID_CORE1:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_rx_core1)
                usb_packet_rx_core1(ctrl, len);
            else
                WARN("USB VEND0: core1 packet received but core1 callback is not set");
		break;
		case USB_PACKET_ID_TTY1:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_rx_tty1)
                usb_packet_rx_tty1(ctrl, len);
            else
                WARN("USB VEND0: tty1 packet received but tty1 callback is not set");
		break;
		case USB_PACKET_ID_TTY2:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_rx_tty2)
                usb_packet_rx_tty2(ctrl, len);
            else
                WARN("USB VEND0: tty2 packet received but tty2 callback isnot set");
		break;
		case USB_PACKET_ID_TTY3:
        	tud_vendor_n_read(0, (void *)&vend_packet, len);
            if (usb_packet_rx_tty3)
                usb_packet_rx_tty3(ctrl, len);
            else
                WARN("USB VEND0: tty3 packet received but tty3 callback is not set");
		break;
		default:
            ERR("USB VEND0: Unknown packet");
		break;
	}
}

//--------------------------------------------------------------------+
// tx
//--------------------------------------------------------------------+

static void usb_tx_packet(void)
{
}

bool usb_timestamp_req(datetime_t *t) {

    return 0;
}

//--------------------------------------------------------------------+
// vendor0 chardev
//--------------------------------------------------------------------+

bool usb_vend0_chardev_connected = false;

static uint8_t usb_vend_read(uint8_t tty)
{
	// TODO unused?
	if (tud_vendor_n_mounted(0))
	{
		if (tud_vendor_n_available(0) > 0) {
			uint8_t b = 0;
			tud_vendor_n_read(0, &b, 1);
			return b;
		}
		else
			WARN("VEND0 buffer full");
	}
	else
	{
		WARN("VEND0 not connected");
	}
	return 0;
}

static void usb_vend_write(uint8_t tty, uint8_t b)
{
	if (tud_vendor_n_mounted(0))
	{
		if (tud_vendor_n_write_available(0) > 2) {
			uint8_t pid = USB_PACKET_ID_TTY1;
			if (tty==2)
				pid = USB_PACKET_ID_TTY2;
			if (tty==3)
				pid = USB_PACKET_ID_TTY3;
			uint8_t pkt[3] = { 2, pid, b };
			tud_vendor_n_write(0, &b, 3);
		}
		else
			WARN("VEND0 buffer full");
	}
	else
		WARN("VEND0 not connected");
}

static bool usb_vend_writable(uint8_t tty)
{
	if (tud_vendor_n_mounted(0))
	{
		return (tud_vendor_n_write_available(0) > 2);
	}
	return false;
}

uint8_t usb_vend_tty1_read(void)
{
	return usb_vend_read(1);
}

void usb_vend_tty1_write(uint8_t b)
{
	usb_vend_write(1, b);
}

bool usb_vend_tty1_writable(void)
{
	return usb_vend_writable(1);
}

uint8_t usb_vend_tty2_read(void)
{
	return usb_vend_read(2);
}

void usb_vend_tty2_write(uint8_t b)
{
	usb_vend_write(2, b);
}

bool usb_vend_tty2_writable(void)
{
	return usb_vend_writable(2);
}

uint8_t usb_vend_tty3_read(void)
{
	return usb_vend_read(3);
}

void usb_vend_tty3_write(uint8_t b)
{
	usb_vend_write(3, b);
}

bool usb_vend_tty3_writable(void)
{
	return usb_vend_writable(3);
}

//--------------------------------------------------------------------+
// vendor0 blockdev
//--------------------------------------------------------------------+

bool usb_vend0_blockdev_connected = false;

uint32_t usb_disk_lba_req(uint8_t disk_id)
{
    // usb_tx_packet(void);

	return 0;
}

//--------------------------------------------------------------------+
// vendor0 callbacks
//--------------------------------------------------------------------+

usb_packet_chardev_fptr usb_packet_rx_core1 = NULL;
usb_packet_chardev_fptr usb_packet_rx_tty1 = NULL;
usb_packet_chardev_fptr usb_packet_rx_tty2 = NULL;
usb_packet_chardev_fptr usb_packet_rx_tty3 = NULL;

void usb_vend0_set_cb(usb_packet_chardev_fptr rx_packet_core1_cb,
						usb_packet_chardev_fptr rx_packet_tty1_cb,
						usb_packet_chardev_fptr rx_packet_tty2_cb,
						usb_packet_chardev_fptr rx_packet_tty3_cb
						) {
	usb_packet_rx_core1 = rx_packet_core1_cb;
	usb_packet_rx_tty1 = rx_packet_tty1_cb;
	usb_packet_rx_tty2 = rx_packet_tty2_cb;
	usb_packet_rx_tty3 = rx_packet_tty3_cb;
}
