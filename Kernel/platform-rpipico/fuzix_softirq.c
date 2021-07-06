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
                INFO("fuzix_softirq CORE1 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_UART0:
                INFO("fuzix_softirq UART0 sig %d count %d", irq.sig, irq.count);
                tty1_inproc(irq.sig);
            break;
            case DEV_ID_UART1:
                INFO("fuzix_softirq UART1 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_I2C0:
                INFO("fuzix_softirq I2C0 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_I2C1:
                INFO("fuzix_softirq I2C1 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_SPI0:
                INFO("fuzix_softirq SPI0 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_SPI1:
                INFO("fuzix_softirq SPI1 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_FLASH:
                //INFO("fuzix_softirq FLASH sig %d count %d\n", irq.sig, irq.count);
                flash_irq_done = true;
            break;
            case DEV_ID_SD:
                INFO("fuzix_softirq SD sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_USB_CDC0:
                INFO("fuzix_softirq CDC0 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_USB_CDC1:
                INFO("fuzix_softirq CDC1 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_USB_CDC2:
                INFO("fuzix_softirq CDC2 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_USB_CDC3:
                INFO("fuzix_softirq CDC3 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_USB_VEND0:
                INFO("fuzix_softirq VEND0 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_USB_VEND1:
                INFO("fuzix_softirq VEND1 sig %d count %d\n", irq.sig, irq.count);
            break;
            case DEV_ID_USB_VEND2:
                INFO("fuzix_softirq VEND2 sig %d count %d\n", irq.sig, irq.count);
            break;
            default:
                ERR("fuzix_softirq unknown irq sig %d count %d\n", irq.sig, irq.count);
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
