#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_chardev.h"
#include "rt_blockdev.h"
#include "rt_core1.h"
#include "rt_uart.h"
#include "rt_usb.h"
#include "rt_blockdev_flash.h"
#include "rt_blockdev_sd.h"
#include "rt_blockdev_usb.h"

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
            case DEV_ID_TIMER:
                WARNING("softirq_timer_handler TIMER sig %d count %d", irq.sig, irq.count);
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
                if (irq.sig == SIG_ID_TRANSFER_REQ) {
                	//stdio_printf("\nSD TRANSFER\n");
                    uint_fast8_t res = blockdev[blockdev_id_sd].transfer();
                    softirq_t irq;
                    mk_softirq(&irq, DEV_ID_SD, res, 0, NULL);
                    // queue softirq
                    if (!queue_try_add(&softirq_out_q, &irq))
                    {
                        // TODO queue full error -> lag -> data lost
                    }
                }
                else if (irq.sig == SIG_ID_TRIM_REQ)
                {
                	//stdio_printf("\nSD TRIM\n");
                    int res = blockdev[blockdev_id_sd].trim();
                    softirq_t irq;
                    mk_softirq(&irq, DEV_ID_SD, res, 0, NULL);
                    // queue softirq
                    if (!queue_try_add(&softirq_out_q, &irq))
                    {
                        // TODO queue full error -> lag -> data lost
                    }
                }
            break;
            case DEV_ID_USB_VEND0:
                INFO("softirq_timer_handler VEND0 sig %d count %d", irq.sig, irq.count);
                if (irq.sig == SIG_ID_TRANSFER_REQ) {
                	//stdio_printf("\nUSB VEND0 TRANSFER\n");
                    uint_fast8_t res = blockdev[blockdev_id_usb_vend0].transfer();
                    softirq_t irq;
                    mk_softirq(&irq, DEV_ID_USB_VEND0, res, 0, NULL);
                    // queue softirq
                    if (!queue_try_add(&softirq_out_q, &irq))
                    {
                        // TODO queue full error -> lag -> data lost
                    }
                }
                else if (irq.sig == SIG_ID_TRIM_REQ)
                {
                	//stdio_printf("\nSD TRIM\n");
                    int res = blockdev[blockdev_id_usb_vend0].trim();
                    softirq_t irq;
                    mk_softirq(&irq, DEV_ID_USB_VEND0, res, 0, NULL);
                    // queue softirq
                    if (!queue_try_add(&softirq_out_q, &irq))
                    {
                        // TODO queue full error -> lag -> data lost
                    }
                }
            break;
            case DEV_ID_STDIO:
                //INFO("softirq_timer_handler STDIO sig %d count %d", irq.sig, irq.count);
                if (irq.sig == SIG_ID_RX) {
                    WARNING("softirq_timer_handler STDIO sig %d count %d", irq.sig, irq.count);
                }
                else if (irq.sig == SIG_ID_TX)
                {
                    putchar(stdio_byte);
                    stdio_irq_done = true;
                }
            break;
            case DEV_ID_TTY1:
                //INFO("softirq_timer_handler TTY1 sig %d count %d", irq.sig, irq.count);
                if (irq.sig == SIG_ID_RX) {
                    tty1_byte = tty1_select_read();
                    tty1_irq_done = true;
                }
                else if (irq.sig == SIG_ID_TX)
                {
                    if (tty1_byte == '\n')
                        tty1_select_write('\r');
                    tty1_select_write(tty1_byte);
                    tty1_irq_done = true;
                }
            break;
            case DEV_ID_TTY2:
                //INFO("softirq_timer_handler TTY2 sig %d count %d", irq.sig, irq.count);
                if (irq.sig == SIG_ID_RX) {
                    tty2_byte = tty2_select_read();
                    tty2_irq_done = true;
                }
                else if (irq.sig == SIG_ID_TX)
                {
                    if (tty2_byte == '\n')
                        tty2_select_write('\r');
                    tty2_select_write(tty2_byte);
                    tty2_irq_done = true;
                }
            break;
            case DEV_ID_TTY3:
                //INFO("softirq_timer_handler TTY3 sig %d count %d", irq.sig, irq.count);
                if (irq.sig == SIG_ID_RX) {
                    tty3_byte = usb_cdc2_read();
                    tty3_irq_done = true;
                }
                else if (irq.sig == SIG_ID_TX )
                {
                    if (tty3_byte == '\n')
                        usb_cdc2_write('\r');
                    usb_cdc2_write(tty3_byte);
                    tty3_irq_done = true;
                }
            break;
            case DEV_ID_TTY4:
                //INFO("softirq_timer_handler TTY4 sig %d count %d", irq.sig, irq.count);
                if (irq.sig == SIG_ID_RX) {
                    tty4_byte = usb_cdc2_read();
                    tty4_irq_done = true;
                }
                else if (irq.sig == SIG_ID_TX )
                {
                    if (tty4_byte == '\n')
                        usb_cdc3_write('\r');
                    usb_cdc3_write(tty4_byte);
                    tty4_irq_done = true;
                }
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
