#include "platform.h"

// signal fuzix is in 'ei'/'irqrestore', ie: core code can be used freely
bool fuzix_ready = true;

void fuzix_softirq(void) {
	while (!queue_is_empty(&softirq_out_q)) {
        softirq_t irq;
        queue_remove_blocking(&softirq_out_q, &irq);
        switch (irq.dev) {
            case DEV_ID_TIMER:
                timer_interrupt();
            break;
            case DEV_ID_CORE1:
            break;
            case DEV_ID_UART0:
                tty1_inproc(irq.sig);
            break;
            case DEV_ID_UART1:
            break;
            case DEV_ID_I2C0:
            break;
            case DEV_ID_I2C1:
            break;
            case DEV_ID_SPI0:
            break;
            case DEV_ID_SPI1:
            break;
            case DEV_ID_FLASH:
            break;
            case DEV_ID_SD:
            break;
            case DEV_ID_USB_CDC0:
            break;
            case DEV_ID_USB_CDC1:
            break;
            case DEV_ID_USB_CDC2:
            break;
            case DEV_ID_USB_CDC3:
            break;
            case DEV_ID_USB_VEND0:
            break;
            case DEV_ID_USB_VEND1:
            break;
            case DEV_ID_USB_VEND2:
            break;
            default:
                // TODO error unknown irq
            break;
        }
        clear_softirq(&irq);
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
