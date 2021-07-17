#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_blockdev.h"
#include "rt_blockdev_usb.h"
#include "rt_core1.h"
#include "rt_fuzix.h"
#include "rt_usb_mplex.h"

#include <tusb.h>

bool usb_vend0_connected = false;
bool usb_vend0_chardev_enabled = false;
bool usb_vend0_blockdev_enabled = false;

//--------------------------------------------------------------------+
// vendor0 rx
//--------------------------------------------------------------------+

uint8_t vend_expected = 0;

static void usb_packet_rx_control(uint8_t len) {
	uint8_t b = 0;
    tud_vendor_n_read(0, &b, len);

    switch (b) {
        case USB_CTRL_ID_CONNECT:
            WARN("USB VEND0: USB_CTRL_ID_CONNECT NOT IMPLEMENTED");
			usb_vend0_connected = true;
        break;
        case USB_CTRL_ID_DISCONNECT:
            WARN("USB VEND0: USB_CTRL_ID_DISCONNECT command NOT IMPLEMENTED");
			usb_vend0_connected = false;
        break;
        case USB_CTRL_ID_BLOCKDEV_CONNECT:
            WARN("USB VEND0: USB_CTRL_ID_BLOCKDEV_CONNECT command NOT IMPLEMENTED");
			usb_vend0_blockdev_enabled = true;
        break;
        case USB_CTRL_ID_BLOCKDEV_DISCONNECT:
            WARN("USB VEND0: USB_CTRL_ID_BLOCKDEV_DISCONNECT command NOT IMPLEMENTED");
			usb_vend0_blockdev_enabled = false;
        break;
        case USB_CTRL_ID_CHARDEV_CONNECT:
            WARN("USB VEND0: USB_CTRL_ID_CHARDEV_CONNECT command NOT IMPLEMENTED");
			usb_vend0_chardev_enabled = true;
        break;
        case USB_CTRL_ID_CHARDEV_DISCONNECT:
            WARN("USB VEND0: USB_CTRL_ID_CHARDEV_DISCONNECT command NOT IMPLEMENTED");
			usb_vend0_chardev_enabled = false;
        break;
        case USB_CTRL_ID_REBOOT:
            WARN("USB VEND0: USB_CTRL_ID_REBOOT NOT IMPLEMENTED");
            // TODO fuzix clean reboot?
        break;
        default:
            ERR("USB VEND0: Unknown ctrl command (%d)", b);
        break;
    }
}

void usb_rx_packet_set_size(void) {
	tud_vendor_n_read(0, &vend_expected, 1);
}

void usb_rx_packet_dispatch(uint8_t len) {
    uint8_t select;
	tud_vendor_n_read(0, (void *)&select, 1);
    len--;
	bool ctrl = (select >> USB_PACKET_CTRL_BIT) & 1U;
	uint8_t b = 0;

	switch (select &= ~(1UL << USB_PACKET_CTRL_BIT)) {
		case USB_PACKET_ID_CTRL:
            usb_packet_rx_control(len);
		break;
		case USB_PACKET_ID_DISK1:
            tud_vendor_n_read(0, blockdev[blockdev_id_usb_disk1].op->addr, len);
			softirq_out(DEV_ID_USB_VEND0, 1, 0, NULL);
		break;
		case USB_PACKET_ID_DISK2:
            tud_vendor_n_read(0, blockdev[blockdev_id_usb_disk2].op->addr, len);
			softirq_out(DEV_ID_USB_VEND0, 2, 0, NULL);
		break;
		case USB_PACKET_ID_DISK3:
            tud_vendor_n_read(0, blockdev[blockdev_id_usb_disk3].op->addr, len);
			softirq_out(DEV_ID_USB_VEND0, 3, 0, NULL);
		break;
		case USB_PACKET_ID_CORE1:
			for (int i=0;i<len;i++) {
	        	tud_vendor_n_read(0, &b, 1);
				core1_write(b);
			}
		break;
		case USB_PACKET_ID_TTY1:
			for (int i=0;i<len;i++) {
	        	tud_vendor_n_read(0, &b, 1);
				softirq_out(DEV_ID_TTY1, b, 0, NULL);
			}
		break;
		case USB_PACKET_ID_TTY2:
			for (int i=0;i<len;i++) {
	        	tud_vendor_n_read(0, &b, 1);
				softirq_out(DEV_ID_TTY2, b, 0, NULL);
			}
		break;
		case USB_PACKET_ID_TTY3:
			for (int i=0;i<len;i++) {
	        	tud_vendor_n_read(0, &b, 1);
				softirq_out(DEV_ID_TTY3, b, 0, NULL);
			}
		break;
		default:
            ERR("USB VEND0: Unknown packet");
		break;
	}
}

//--------------------------------------------------------------------+
// vendor0 tx
//--------------------------------------------------------------------+

static void usb_tx_packet(void)
{
}

bool usb_connection_req(void) {
	// TODO perform handshaking
    return false;
}

bool usb_datetime_req(datetime_t *t) {
	// TODO get date and time and fill t
    return false;
}

//--------------------------------------------------------------------+
// vendor0 chardev
//--------------------------------------------------------------------+

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

uint32_t usb_disk_lba_req(uint8_t disk_id)
{
    // usb_tx_packet(void);
	return 0;
}

bool usb_disk_read_req(uint8_t disk_id, uint32_t lba, uint8_t *addr)
{
    // usb_tx_packet(void);
    return false;
}

bool usb_disk_write_req(uint8_t disk_id, uint32_t lba, uint8_t *addr)
{
    // usb_tx_packet(void);
    return false;
}

bool usb_disk_trim_req(uint8_t disk_id, uint32_t lba, uint8_t *addr)
{
    // usb_tx_packet(void);
    return false;
}
