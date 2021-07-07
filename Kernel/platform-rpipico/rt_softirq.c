#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_chardev.h"
#include "rt_blockdev.h"
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
    //NOTICE("softirq_timer_handler");
	while (!queue_is_empty(&softirq_in_q)) {
        softirq_t irq;
        queue_remove_blocking(&softirq_in_q, &irq);
        switch (irq.dev) {
            case DEV_ID_STDIO:
            break;
            case DEV_ID_TIMER:
                INFO("softirq_timer_handler TIMER sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_CORE1:
                INFO("softirq_timer_handler CORE1 sig %d count %d", irq.sig, irq.count);
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
                INFO("softirq_timer_handler UART0 sig %d count %d", irq.sig, irq.count);
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
                INFO("softirq_timer_handler UART1 sig %d count %d", irq.sig, irq.count);
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
                INFO("softirq_timer_handler I2C0 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_I2C1:
                INFO("softirq_timer_handler I2C1 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_SPI0:
                INFO("softirq_timer_handler SPI0 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_SPI1:
                INFO("softirq_timer_handler SPI1 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_FLASH:
                //INFO("softirq_timer_handler FLASH sig %d count %d", irq.sig, irq.count);
                if (irq.sig == SIG_ID_TRANSFER_REQ) {
                	//stdio_printf("\nFLASH TRANSFER\n");
                    uint_fast8_t res = blockdev[blockdev_id_flash].transfer();
                    softirq_t irq;
                    mk_softirq(&irq, DEV_ID_FLASH, res, 0, NULL);
                    // queue softirq
                    if (!queue_try_add(&softirq_out_q, &irq))
                    {
                        // TODO queue full error -> lag -> data lost
                    }
                }
                else if (irq.sig == SIG_ID_TRIM_REQ)
                {
                	//stdio_printf("\nFLASH TRIM\n");
                    int res = blockdev[blockdev_id_flash].trim();
                    softirq_t irq;
                    mk_softirq(&irq, DEV_ID_FLASH, res, 0, NULL);
                    // queue softirq
                    if (!queue_try_add(&softirq_out_q, &irq))
                    {
                        // TODO queue full error -> lag -> data lost
                    }
                }
            break;
            case DEV_ID_SD:
                INFO("softirq_timer_handler SD sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_USB_CDC0:
                INFO("softirq_timer_handler CDC0 sig %d count %d", irq.sig, irq.count);
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
                INFO("softirq_timer_handler CDC1 sig %d count %d", irq.sig, irq.count);
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
                INFO("softirq_timer_handler CDC2 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_USB_CDC3:
                INFO("softirq_timer_handler CDC3 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_USB_VEND0:
                INFO("softirq_timer_handler VEND0 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_USB_VEND1:
                INFO("softirq_timer_handler VEND1 sig %d count %d", irq.sig, irq.count);
            break;
            case DEV_ID_USB_VEND2:
                INFO("softirq_timer_handler VEND2 sig %d count %d", irq.sig, irq.count);
            break;
            default:
                ERR("softirq_timer_handler unknown irq sig %d count %d", irq.sig, irq.count);
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
