#include "platform.h"

// signal fuzix is in 'ei'/'irqrestore', ie: core code can be used freely
bool fuzix_ready = true;

void fuzix_softirq(void) {
    //NOTICE("fuzix_softirq");
	while (!queue_is_empty(&softirq_out_q)) {
        softirq_t irq;
        queue_remove_blocking(&softirq_out_q, &irq);
        switch (irq.dev) {
            case DEV_ID_TICKER:
                timer_interrupt();
            break;
            case DEV_ID_CORE1:
                INFO("fuzix_softirq CORE1 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_FLASH:
                flash_irq_done = true;
            break;
            case DEV_ID_SD:
                INFO("fuzix_softirq SD sig %d count %d", irq.sig, irq.count);
                sd_irq_done = true;
            break;
            case DEV_ID_USB_VEND0:
                INFO("fuzix_softirq VEND0 sig %d count %d", irq.sig, irq.count);
                usb_irq_done = true;
            break;
            case DEV_ID_TTY1:
                tty_inproc(minor((512 + 1)), irq.sig);
            break;
            case DEV_ID_TTY2:
                tty_putc(minor((512 + 2)), irq.sig);
            break;
            case DEV_ID_TTY3:
                tty_putc(minor((512 + 3)), irq.sig);
            break;
            default:
                ERR("fuzix_softirq unknown irq sig %d count %d", irq.sig, irq.count);
            break;
	    }
    }
}

uint32_t di(void) {
	fuzix_ready = false;
	return 0;
}

void ei(void) {
	fuzix_ready = true;
	fuzix_softirq();
}

void irqrestore(uint32_t ps) {
	fuzix_ready = true;
	fuzix_softirq();
}

/* vim: sw=4 ts=4 et: */
