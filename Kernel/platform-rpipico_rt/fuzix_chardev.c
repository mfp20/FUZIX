#include "platform.h"

#include <kernel.h>
#include <blkdev.h>

//--------------------------------------------------------------------+
// chardev drivers
//--------------------------------------------------------------------+

static uint_fast8_t chardev_signal(uint8_t dev, uint8_t req, bool *flag) {
	// evelope chardev request for softirq
	softirq_in(dev, req, 0, NULL);

	// wait for response
	while (!(*flag)) {
		fuzix_softirq();
	}
	*flag = false;

	return 1; // success
}

static uint8_t virtual_stdio_read(void) {
	chardev_signal(DEV_ID_STDIO, SIG_ID_RX, &stdio_irq_done);
	return stdio_byte;
}

static void virtual_stdio_write(uint8_t b) {
	stdio_byte = b;
	chardev_signal(DEV_ID_STDIO, SIG_ID_TX, &stdio_irq_done);
}

static bool virtual_stdio_writable(void) {
	return !stdio_irq_done;
}

static uint8_t virtual_tty1_read(void) {
	chardev_signal(DEV_ID_TTY1, SIG_ID_RX, &tty1_irq_done);
	return tty1_byte;
}

static void virtual_tty1_write(uint8_t b) {
	tty1_byte = b;
	chardev_signal(DEV_ID_TTY1, SIG_ID_TX, &tty1_irq_done);
}

static bool virtual_tty1_writable(void) {
	return !tty1_irq_done;
}

static uint8_t virtual_tty2_read(void) {
	chardev_signal(DEV_ID_TTY2, SIG_ID_RX, &tty2_irq_done);
	return tty2_byte;
}

static void virtual_tty2_write(uint8_t b) {
	tty2_byte = b;
	chardev_signal(DEV_ID_TTY2, SIG_ID_TX, &tty2_irq_done);
}

static bool virtual_tty2_writable(void) {
	return !tty2_irq_done;
}

static uint8_t virtual_tty3_read(void) {
	chardev_signal(DEV_ID_TTY3, SIG_ID_RX, &tty3_irq_done);
	return tty3_byte;
}

static void virtual_tty3_write(uint8_t b) {
	tty3_byte = b;
	chardev_signal(DEV_ID_TTY3, SIG_ID_TX, &tty3_irq_done);
}

static bool virtual_tty3_writable(void) {
	return !tty3_irq_done;
}

//--------------------------------------------------------------------+
// init
//--------------------------------------------------------------------+

void virtual_stdio_init(void) {
	// uart0 early init
	uart0_init(0, 1, 115200, NULL);

	// init pico stdio
	stdio_init_all();
	stdio_set_driver_enabled(&stdio_driver, true);

	// virtual chardevs
	tty_cd[0] = chardev_add(virtual_stdio_read, virtual_stdio_write, virtual_stdio_writable);
	tty_cd[1] = chardev_add(virtual_tty1_read, virtual_tty1_write, virtual_tty1_writable);
	tty_cd[2] = chardev_add(virtual_tty2_read, virtual_tty2_write, virtual_tty2_writable);
	tty_cd[3] = chardev_add(virtual_tty3_read, virtual_tty3_write, virtual_tty3_writable);
}
