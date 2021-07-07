#include "platform.h"

#include <kernel.h>
#include <blkdev.h>

static uint_fast8_t chardev_signal(uint8_t dev, uint8_t req, bool *flag) {
	// evelope chardev reqeust for softirq
	softirq_t irq;
	mk_softirq(&irq, dev, req, 0, NULL);

	// queue request
	while (!queue_try_add(&softirq_in_q, &irq)) ; // TODO count queue lag

	// wait for response
	while (!(*flag)) {
		fuzix_softirq();
	}
	*flag = false;

	return 1; // success
}

static uint_fast8_t chardev_stdio_signal(uint8_t req) {
	return chardev_signal( DEV_ID_STDIO, req, &stdio_irq_done);
}

static uint_fast8_t chardev_tty1_signal(uint8_t req) {
	chardev_signal( DEV_ID_TTY1, req, &tty1_irq_done);
}

static uint_fast8_t chardev_tty2_signal(uint8_t req) {
	chardev_signal( DEV_ID_TTY2, req, &tty2_irq_done);
}

static uint_fast8_t chardev_tty3_signal(uint8_t req) {
	chardev_signal( DEV_ID_TTY3, req, &tty3_irq_done);
}

static uint_fast8_t chardev_tty4_signal(uint8_t req) {
	chardev_signal( DEV_ID_TTY4, req, &tty4_irq_done);
}

static uint8_t virtual_stdio_read(void) {
	return chardev_stdio_signal(SIG_ID_RX);
}

static void virtual_stdio_write(uint8_t b) {
	stdio_byte = b;
	chardev_stdio_signal(SIG_ID_TX);
}

static bool virtual_stdio_writable(void) {
	return !stdio_irq_done;
}

static uint8_t virtual_tty1_read(void) {
	chardev_tty1_signal(SIG_ID_RX);
	return tty1_byte;
}

static void virtual_tty1_write(uint8_t b) {
	//kprintf("virtual_tty1_write called\n");
	tty1_byte = b;
	chardev_tty1_signal(SIG_ID_TX);
}

static bool virtual_tty1_writable(void) {
	return !tty1_irq_done;
}

static uint8_t virtual_tty2_read(void) {
	chardev_tty2_signal(SIG_ID_RX);
	return tty2_byte;
}

static void virtual_tty2_write(uint8_t b) {
	tty2_byte = b;
	chardev_tty2_signal(SIG_ID_TX);
}

static bool virtual_tty2_writable(void) {
	return !tty2_irq_done;
}

static uint8_t virtual_tty3_read(void) {
	chardev_tty3_signal(SIG_ID_RX);
	return tty3_byte;
}

static void virtual_tty3_write(uint8_t b) {
	tty3_byte = b;
	chardev_tty3_signal(SIG_ID_TX);
}

static bool virtual_tty3_writable(void) {
	return !tty3_irq_done;
}

static uint8_t virtual_tty4_read(void) {
	chardev_tty4_signal(SIG_ID_RX);
	return tty4_byte;
}

static void virtual_tty4_write(uint8_t b) {
	tty4_byte = b;
	chardev_tty4_signal(SIG_ID_TX);
}

static bool virtual_tty4_writable(void) {
	return !tty4_irq_done;
}

void virtual_stdio_init(void) {
	// init pico stdio
	stdio_init_all();

	// uart0: early init
	uart0_init(0, 1, 115200, NULL);
	
	// uart0: early log
	uart_stdio(0, true, true);
}

// UART0/CDC0: getty
void virtual_tty1_init(void) {
	tty_cd[1] = chardev_add(virtual_tty1_read, virtual_tty1_write, virtual_tty1_writable);
	//kprintf("tty1 init as chardev[%d]\n", tty_cd[1]);
}

// UART0/CDC1: console (kputchar)
void virtual_tty2_init(void) {
	tty_cd[2] = chardev_add(virtual_tty2_read, virtual_tty2_write, virtual_tty2_writable);
	//kprintf("tty2 init as chardev[%d]\n", tty_cd[2]);
}

// CDC2: user tty
void virtual_tty3_init(void) {
	tty_cd[3] = chardev_add(virtual_tty3_read, virtual_tty3_write, virtual_tty3_writable);
	//kprintf("tty3 init as chardev[%d]\n", tty_cd[3]);
}

// CDC3: user tty
void virtual_tty4_init(void) {
	tty_cd[4] = chardev_add(virtual_tty4_read, virtual_tty4_write, virtual_tty4_writable);
	//kprintf("tty4 init as chardev[%d]\n", tty_cd[4]);
}
