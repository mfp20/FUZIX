#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_core1.h"
#include "rt_uart.h"
#include "rt_usb.h"
#include "rt_flash.h"

#include <stdlib.h>

static repeating_timer_t softirq_timer;

pico_queue_t softirq_in_q;
pico_queue_t softirq_out_q;

bool mk_softirq(softirq_t *irq, uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data) {
    irq->dev = dev_id;
    irq->sig = signal_id;
    irq->count = count;
    if (count) {
        irq->data = data;
    }
    return true;
}
void clear_softirq(softirq_t *irq) {
    if (irq->count)
        free(irq->data);
}

static bool softirq_timer_handler(repeating_timer_t *rt)
{
	while (!queue_is_empty(&softirq_in_q)) {
        softirq_t irq;
        queue_remove_blocking(&softirq_in_q, &irq);
        switch (irq.dev) {
            case DEV_ID_TIMER:
                // no input
            break;
            case DEV_ID_CORE1:
                if (irq.count) {
                    // TODO
                }
                else
                {
                    if (core1_writable()) {
                        core1_write(irq.sig);
                    }
                }
            break;
            case DEV_ID_UART0:
                if (irq.count) {
                    // TODO
                }
                else
                {
                    if (uart0_writable()) {
                        uart0_write(irq.sig);
                    }
                }
            break;
            case DEV_ID_UART1:
                if (irq.count) {
                    // TODO
                }
                else
                {
                    if (uart1_writable()) {
                        uart1_write(irq.sig);
                    }
                }
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
                if (irq.count) {
                    // TODO
                }
                else
                {
                    if (usb_cdc0_writable()) {
                        usb_cdc0_write(irq.sig);
                    }
                }
            break;
            case DEV_ID_USB_CDC1:
                if (irq.count) {
                    // TODO
                }
                else
                {
                    if (usb_cdc1_writable()) {
                        usb_cdc1_write(irq.sig);
                    }
                }
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
	return true;
}

void softirq_init(void) {
    queue_init(&softirq_in_q, sizeof(softirq_t), UINT8_MAX);
	queue_init(&softirq_out_q, sizeof(softirq_t), UINT8_MAX);

	add_repeating_timer_us(((1000000 / TICKSPERSEC)/2)+1, softirq_timer_handler, NULL, &softirq_timer);
}
