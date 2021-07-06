#include "platform.h"

#include <version.h>
#include <kernel.h>
#include <kdata.h>
#include <devsys.h>
#include <blkdev.h>
#include <tty.h>
#include <printf.h>
#include <dev/devsd.h>

// The device driver switch table
struct devsw dev_tab[] = {
	// minor    open         close        read      write           ioctl
	// ---------------------------------------------------------------------
	/* 0: /dev/hd - block device interface */
	{blkdev_open, no_close, blkdev_read, blkdev_write, blkdev_ioctl},
	/* 1: /dev/fd - Floppy disk block devices */
	{no_open, no_close, no_rdwr, no_rdwr, no_ioctl},
	/* 2: /dev/tty	TTY devices */
	{tty_open, tty_close, tty_read, tty_write, tty_ioctl},
	/* 3: /dev/lpr	Printer devices */
	{no_open, no_close, no_rdwr, no_rdwr, no_ioctl},
	/* 4: /dev/mem etc	System devices (one offs) */
	{no_open, no_close, sys_read, sys_write, sys_ioctl},
	/* Pack to 7 with nxio if adding private devices and start at 8 */
};

bool validdev(uint16_t dev)
{
	// This is a bit uglier than needed but the right hand side is a constant this way
	if (dev > ((sizeof(dev_tab) / sizeof(struct devsw)) << 8) - 1)
		return false;
	else
		return true;
}

void device_init(void)
{
	// power level
	//power_set_mode(POWER_DEFAULT);

	// time
	ticker_init();

	// usb
	usb_init();

	// flash device is mounted first and will be the fallback root device in the case
	// no SD and no USB external filesystems are found.
	virtual_flash_init();

	// SD, if any
	devsd_spi_init();
	devsd_init();
	//virtual_sd_init();

	// USB, if any
	// virtual_usb_fs_init();

	// led on, signal hw init complete
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, 1);
}

/* vim: sw=4 ts=4 et: */
