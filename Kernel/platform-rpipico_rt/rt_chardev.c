#include "rt_log.h"
#include "rt_chardev.h"
#include "rt_uart.h"
#include "rt_usb.h"

#include <tusb.h>

#include <stdlib.h>

// all available chardevs
chardev_t *chardev = NULL;
static uint8_t chardev_no = 0;

bool stdio_irq_done = false;
bool tty1_irq_done = false;
bool tty2_irq_done = false;
bool tty3_irq_done = false;
bool tty4_irq_done = false;

uint8_t stdio_byte = 0;
uint8_t tty1_byte = 0;
uint8_t tty2_byte = 0;
uint8_t tty3_byte = 0;
uint8_t tty4_byte = 0;

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

uint8_t tty1_select_read(void)
{
	if (tud_cdc_n_connected(0))
	{
		return usb_cdc0_read();
	}
	return uart0_read();
}

void tty1_select_write(uint8_t b)
{
	//stdio_printf("tty1_select_write %c\n", b);
	if (tud_cdc_n_connected(0))
	{
		//stdio_printf("tty1_select_write cdc0 connected\n");
		usb_cdc0_write(b);
		return;
	}
	uart0_write(b);
}

bool tty1_select_writable(void)
{
	if (tud_cdc_n_connected(0))
	{
		return usb_cdc0_writable();
	}
	return uart0_writable();
}

uint8_t tty2_select_read(void)
{
	if (tud_cdc_n_connected(1))
	{
		return usb_cdc1_read();
	}
	return uart0_read();
}

void tty2_select_write(uint8_t b)
{
	if (tud_cdc_n_connected(1))
	{
		usb_cdc1_write(b);
		return;
	}
	uart0_write(b);
}

bool tty2_select_writable(void)
{
	if (tud_cdc_n_connected(1))
	{
		return usb_cdc1_writable();
	}
	return uart0_writable();
}

/* vim: sw=4 ts=4 et: */
