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

void devvirt_service(void) {
	softirq_t irq;
    iop_t *iop;
    // priority queue, immediate processing for all queued zero-bytes signals
	while (queue_get_level(&devvirt_signal_q)>0) {
		queue_remove_blocking(&devvirt_signal_q, &irq);
		switch (irq.id) {
			case IRQ_ID_TICK:
    			timer_interrupt();
			break;
			default:
				// TODO error unknown irq
			break;
        }
    }
    // fast queue, max 64 bytes (the rest in platform_idle)
    uint8_t count = 0;
	while (queue_get_level(&devvirt_byte_q)>0) {
		queue_remove_blocking(&devvirt_byte_q, &irq);
		if (irq.id != IRQ_ID_BYTEDEV) {
            // TODO error unknown IRQ
        } else {
            switch (*((uint8_t*)irq.data)){
                case DEV_ID_UART0:
                    if (*((uint8_t*)irq.data+1) == OP_ID_READ)
                        tty_inproc(minor(BOOT_TTY), *((uint8_t*)irq.data+2));
                    else  
                        ; // TODO uart write
                break;
                case DEV_ID_UART1:
                    // TODO
                break;
                case DEV_ID_USB:
                    // TODO
                break;
                default:
                    // TODO error unknown irq
                break;
            }
        }
        clear_byte_irq(&irq);
        if (count = 64)
            break;
        count++;
    }
    // best effort queue (one block here, the rest in platform_idle)
	if (queue_get_level(&devvirt_block_q)>0) {
		queue_remove_blocking(&devvirt_block_q, &irq);
		if (irq.id != IRQ_ID_BLOCKDEV) {
            // TODO error unknown IRQ
        } else {
            iop = (iop_t *)irq.data;
            switch (iop->dev){
                case DEV_ID_FLASH:
                    // TODO trigger Fuzix
                break;
                case DEV_ID_SD:
                    // TODO trigger Fuzix
                break;
                case DEV_ID_USB:
                    // TODO trigger Fuzix
                break;
                default:
                    // TODO error unknown irq
                break;
            }
		}
        clear_irq(&irq);
	}
}
