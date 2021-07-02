#include "devvirt.h"

#include <stdlib.h>
#include <kernel.h>
#include <tty.h>

pico_queue_t devvirt_signal_q;
pico_queue_t devvirt_byte_q;
pico_queue_t devvirt_block_q;

bool mk_byte_irq(softirq_t *irq, uint8_t irq_id, void *data_addr, uint8_t dev_id, uint8_t op_id, uint8_t byte) {
    irq->id = irq_id;
    if (data_addr) {
        irq->data = data_addr;
        irq->free = false;
    } else {
        irq->data = malloc(3);
        if (irq->data)
            irq->free = true;
        else {
            return false;
        }
    }
    *((uint8_t*)irq->data) = dev_id;
    *((uint8_t*)irq->data+1) = op_id;
    *((uint8_t*)irq->data+2) = byte;
}

void clear_byte_irq(softirq_t *irq) {
    if (irq->free) free(irq->data);
} 

bool mk_irq(softirq_t *irq, uint8_t irq_id, iop_t *iop, uint8_t dev_id, uint8_t op_id, void *iop_data, uint32_t count) {
    irq->id = irq_id;
    if (iop) {
        irq->data = (void *)iop;
        irq->free = false;
    } else {
        irq->data = malloc(sizeof(iop_t));
        if (irq->data)
            irq->free = true;
        else {
            return false;
        }
        iop = (iop_t *)irq->data;
    }
    iop->dev = dev_id;
    iop->op = op_id;
    if (iop_data) {
        iop->data = iop_data;
        iop->free = false;
    } else {
        iop->data = malloc(count);
        if (iop->data)
            iop->free = true;
        else {
            free(iop);
            return false;
        }
    }
    iop->count = count;
    // success!
    return true;
}

void clear_irq(softirq_t *irq) {
    if (((iop_t *)irq->data)->free) free(((iop_t *)irq->data)->data);
    if (irq->free) free(irq->data);
}

void devvirt_pop_signal(void) {
	softirq_t irq;

    queue_remove_blocking(&devvirt_signal_q, &irq);

    if (irq.id != IRQ_ID_SIGNAL) {
        // TODO error unknown IRQ
    } else {
        switch (*((uint8_t*)irq.data)) {
            case DEV_ID_TIMER:
                timer_interrupt();
            break;
            default:
                // TODO error unknown irq
            break;
        }
    }

    clear_byte_irq(&irq);
} 

void devvirt_pop_byte(void) {
	softirq_t irq;

    queue_remove_blocking(&devvirt_byte_q, &irq);

    if (irq.id != IRQ_ID_BYTE) {
        // TODO error unknown IRQ
    } else {
        switch (*((uint8_t*)irq.data)) {
            case DEV_ID_CORE1:
                // TODO
            break;
            case DEV_ID_UART0:
                //if (*((uint8_t*)irq.data+1) == OP_ID_READ)
                //    tty_inproc(minor(BOOT_TTY), *((uint8_t*)irq.data+2));
                //else  
                //    ; // TODO uart write
            break;
            case DEV_ID_UART1:
                // TODO
            break;
            case DEV_ID_I2C0:
                // TODO
            break;
            case DEV_ID_I2C1:
                // TODO
            break;
            case DEV_ID_SPI0:
                // TODO
            break;
            case DEV_ID_SPI1:
                // TODO
            break;
            case DEV_ID_USB_CDC0:
                // TODO
            break;
            case DEV_ID_USB_CDC1:
                // TODO
            break;
            case DEV_ID_USB_CDC2:
                // TODO
            break;
            case DEV_ID_USB_CDC3:
                // TODO
            break;
            default:
                // TODO error unknown irq
            break;
        }
    }

    clear_byte_irq(&irq);
}

void devvirt_pop_block(void) {
	softirq_t irq;
    iop_t *iop;

    queue_remove_blocking(&devvirt_block_q, &irq);

    if (irq.id != IRQ_ID_BLOCK) {
        // TODO error unknown IRQ
    } else {
        iop = (iop_t *)irq.data;
        switch (iop->dev){
            case DEV_ID_FLASH:
                // TODO
            break;
            case DEV_ID_SD:
                // TODO
            break;
            case DEV_ID_USB_VEND0:
                // TODO
            break;
            case DEV_ID_USB_VEND1:
                // TODO
            break;
            case DEV_ID_USB_VEND2:
                // TODO
            break;
            default:
                // TODO error unknown irq
            break;
        }
    }

    clear_irq(&irq);
}

void devvirt_quick(void) {
    // priority queue, immediate processing for all queued signals
	while (!queue_is_empty(&devvirt_signal_q)) {
        devvirt_pop_signal();
    }
    // fast queue, max 64 bytes (the rest in platform_idle)
    uint8_t count = 0;
	while (!queue_is_empty(&devvirt_byte_q)) {
        devvirt_pop_byte();
        if (count == 64)
            break;
        count++;
    }
    // best effort queue, 1 block (the rest in platform_idle)
    if (!queue_is_empty(&devvirt_block_q)) {
        devvirt_pop_block();
	}
}

void devvirt_flush(void) {
    // flush priority queue
	while (!queue_is_empty(&devvirt_signal_q)) {
        devvirt_pop_signal();
    }
    // flush fast queue
	while (!queue_is_empty(&devvirt_byte_q)) {
        devvirt_pop_byte();
    }
    // flush best effort queue
	while (!queue_is_empty(&devvirt_block_q)) {
        devvirt_pop_block();
        switchout();
	}
}
