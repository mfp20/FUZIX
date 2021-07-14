#include "rt_log.h"
#include "rt_chardev.h"
#include "rt_uart.h"
#include "rt_usb.h"

#include <tusb.h>

#include <stdlib.h>

// all available chardevs
chardev_t *chardev = NULL;
static uint8_t chardev_no = 0;

// map chardevs to ttys
uint8_t tty_cd[4] = {0, 1, 2, 3};

//
uint8_t stdio_byte = 0;
uint8_t tty1_byte = 0;
uint8_t tty2_byte = 0;
uint8_t tty3_byte = 0;

//
bool stdio_irq_done = false;
bool tty1_irq_done = false;
bool tty2_irq_done = false;
bool tty3_irq_done = false;

uint8_t chardev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w)
{
	if (chardev_no)
		chardev = realloc(chardev, sizeof(chardev_t) * (chardev_no + 1));
	else
		chardev = malloc(sizeof(chardev_t));
	chardev[chardev_no].rx = r;
	chardev[chardev_no].tx = t;
	chardev[chardev_no].ready = w;
	chardev_no++;

	return chardev_no - 1;
}

void chardev_mod(uint8_t chardev_id, byte_rx_t r, byte_tx_t t, byte_ready_t w)
{
	chardev[chardev_id].rx = r;
	chardev[chardev_id].tx = t;
	chardev[chardev_id].ready = w;
}

//--------------------------------------------------------------------+
// real chardevs
//--------------------------------------------------------------------+

// uart0/cdc0/vend0 selection for kputchar+tty1
uint8_t fuzix_select_read(void)
{
	if (usb_vend0_chardev_connected) {
		return usb_vend_tty1_read();
	}
	if (tud_cdc_n_connected(0))
	{
		return usb_cdc0_read();
	}
	return uart_getc(uart0);
}

void fuzix_select_write(uint8_t b)
{
	if (usb_vend0_chardev_connected) {
		usb_vend_tty1_write(b);
		return;
	}
	if (tud_cdc_n_connected(0))
	{
		usb_cdc0_write(b);
		return;
	}
	uart_putc(uart0, b);
}

bool fuzix_select_writable(void)
{
	if (usb_vend0_chardev_connected) {
		return usb_vend_tty1_writable();
	}
	if (tud_cdc_n_connected(0))
	{
		return usb_cdc0_writable();
	}
	return uart_is_writable(uart0);
}

// uart0/cdc1/vend0 selection for kputchar+tty2
uint8_t rt_select_read(void)
{
	if (usb_vend0_chardev_connected) {
		return usb_vend_tty2_read();
	}
	if (tud_cdc_n_connected(1))
	{
		return usb_cdc1_read();
	}
	return fuzix_select_read();
}

void rt_select_write(uint8_t b)
{
	if (usb_vend0_chardev_connected) {
		usb_vend_tty2_write(b);
		return;
	}
	if (tud_cdc_n_connected(1))
	{
		usb_cdc1_write(b);
		return;
	}
	fuzix_select_write(b);
}

bool rt_select_writable(void)
{
	if (usb_vend0_chardev_connected) {
		return usb_vend_tty2_writable();
	}
	if (tud_cdc_n_connected(1))
	{
		return usb_cdc1_writable();
	}
	return fuzix_select_writable();
}

// cdc2/vend0 selection for tty3
uint8_t tty3_select_read(void)
{
	if (usb_vend0_chardev_connected) {
		return usb_vend_tty3_read();
	}
	if (tud_cdc_n_connected(2))
	{
		return usb_cdc2_read();
	}
	WARN("tty3 not connected");
	return 0;
}

void tty3_select_write(uint8_t b)
{
	if (usb_vend0_chardev_connected) {
		usb_vend_tty3_write(b);
		return;
	}
	if (tud_cdc_n_connected(2))
	{
		usb_cdc2_write(b);
		return;
	}
	WARN("tty3 not connected");
}

bool tty3_select_writable(void)
{
	if (usb_vend0_chardev_connected) {
		return usb_vend_tty3_writable();
	}
	if (tud_cdc_n_connected(2))
	{
		return usb_cdc2_writable();
	}
	WARN("tty3 not connected");
	return false;
}

/* vim: sw=4 ts=4 et: */
