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
  {  blkdev_open,   no_close,   blkdev_read,    blkdev_write,	blkdev_ioctl},
  /* 1: /dev/fd - Floppy disk block devices */
  {  no_open,	    no_close,	no_rdwr,	no_rdwr,	no_ioctl},
  /* 2: /dev/tty	TTY devices */
  {  tty_open,     tty_close,   tty_read,  tty_write,  tty_ioctl },
  /* 3: /dev/lpr	Printer devices */
  {  no_open,     no_close,   no_rdwr,   no_rdwr,  no_ioctl  },
  /* 4: /dev/mem etc	System devices (one offs) */
  {  no_open,      no_close,    sys_read, sys_write, sys_ioctl  },
  /* Pack to 7 with nxio if adding private devices and start at 8 */
};

bool validdev(uint16_t dev) {
    // This is a bit uglier than needed but the right hand side is a constant this way
    if (dev > ((sizeof(dev_tab)/sizeof(struct devsw)) << 8) - 1)
	    return false;
    else
        return true;
}

static repeating_timer_t systick_timer;

static bool systick_timer_handler(repeating_timer_t *rt) {
    if (fuzix_ready&&queue_is_empty(&devvirt_signal_q)) {
        timer_interrupt();
    } else {
        softirq_t irq;
        // TODO use for something useful the unused 2 bytes
        if (!mk_byte_irq(&irq, IRQ_ID_SIGNAL, NULL, DEV_ID_TIMER, 0, 0)) {
            // TODO out of memory error
            return false;
        }
        // queue softirq
        if (!queue_try_add(&devvirt_signal_q, &irq)) {
            // TODO queue full error -> lag -> data lost
            return false;
        }
    }

    return true;
}

void device_init(void) {
    // power
    //power_set_mode(POWER_DEFAULT);

    // ticker
    add_repeating_timer_us((1000000 / TICKSPERSEC), systick_timer_handler, NULL, &systick_timer);

    // uart1 log
    uart1_init(4, 5, 115200, NULL);
    uart_stdio(1, true);

    // usb
    usb_init();
    uint8_t usb_id = chardev_add(usb_cdc0_read, usb_cdc0_write, usb_cdc0_writable);
    usb_cdc_stdio(1, true);
    //chardev_add(usb_cdc2_read, usb_cdc2_write, usb_cdc2_writable);
    //chardev_add(usb_cdc3_read, usb_cdc3_write, usb_cdc3_writable);

    // Flash device is too small to be useful, and a corrupt flash will cause a crash on startup... oddly.
	devflash_init();

    // SD, if any
	devsd_spi_init();
	devsd_init();

    // led on, signal init complete
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
}

/* vim: sw=4 ts=4 et: */

