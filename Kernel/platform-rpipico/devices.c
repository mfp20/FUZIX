
#include "platform.h"
//#include "devusb.h"

#include <version.h>
#include <kernel.h>
#include <kdata.h>
#include <devsys.h>
#include <blkdev.h>
#include <tty.h>
#include <printf.h>
#include <dev/devsd.h>

//
static absolute_time_t now;

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
    /* This is a bit uglier than needed but the right hand side is
       a constant this way */
    if(dev > ((sizeof(dev_tab)/sizeof(struct devsw)) << 8) - 1)
	return false;
    else
        return true;
}

static void timer_tick_cb(unsigned alarm) {
    absolute_time_t next;
    update_us_since_boot(&next, to_us_since_boot(now) + (1000000 / TICKSPERSEC));
    if (hardware_alarm_set_target(0, next)) {
        update_us_since_boot(&next, time_us_64() + (1000000 / TICKSPERSEC));
        hardware_alarm_set_target(0, next);
    }
    //
    if (fuzix_ready&&queue_is_empty(&devvirt_signal_q)) {
        timer_interrupt();
    } else {
        softirq_t irq;
        // TODO use for something useful the unused 2 bytes
        if (!mk_byte_irq(&irq, IRQ_ID_SIGNAL, NULL, DEV_ID_TIMER, 0, 0)) {
            // TODO out of memory error
            return;
        }
        // queue softirq
        if (!queue_try_add(&devvirt_signal_q, &irq)) {
            // TODO queue full error -> lag -> data lost
        }
    }
}

void device_init(void) {
    /* The flash device is too small to be useful, and a corrupt flash will
     * cause a crash on startup... oddly. */
	devflash_init();

    // SD, if any
	devsd_spi_init();
	devsd_init();

    // usb for external fs
    //devusb_init();

    // ticker
    hardware_alarm_claim(0);
    update_us_since_boot(&now, time_us_64());
    hardware_alarm_set_callback(0, timer_tick_cb);
    timer_tick_cb(0);

    // led on, signal init complete
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
}

/* vim: sw=4 ts=4 et: */

