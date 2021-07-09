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

pico_queue_t softirq_in_q;
pico_queue_t softirq_out_q;

static repeating_timer_t softirq_timer;

static bool mk_softirq(softirq_t *irq, uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data) {
    irq->dev = dev_id;
    irq->sig = signal_id;
    irq->count = count;
    if (count) {
        irq->data = data;
    }
    return true;
}

void irq_out(uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data) {
    softirq_t irq;
    mk_softirq(&irq, dev_id, signal_id, count, data);
    while (!queue_try_add(&softirq_out_q, &irq)) ; // TODO queue full error -> lag -> data lost
}

void irq_in(uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data) {
    softirq_t irq;
    mk_softirq(&irq, dev_id, signal_id, count, data);
    while (!queue_try_add(&softirq_in_q, &irq)) ; // TODO queue full error -> lag -> data lost
}

void clear_softirq(softirq_t *irq) {
    if (irq->count)
        free(irq->data);
}

// process all data from fuzix's virtual devices to real hardware
static bool rt_softirq(repeating_timer_t *rt)
{
    //NOTICE("rt_softirq");
    softirq_t irq_in;
	while (!queue_is_empty(&softirq_in_q)) {
        queue_remove_blocking(&softirq_in_q, &irq_in);
        switch (irq_in.dev) {
            case DEV_ID_TIMER:
                WARNING("rt_softirq TIMER sig %d count %d", irq_in.sig, irq_in.count);
            break;
            case DEV_ID_CORE1:
                INFO("rt_softirq CORE1 sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.count) {
                    WARNING("rt_softirq CORE1 sig %d count %d", irq_in.sig, irq_in.count);
                    // TODO
                }
                else
                {
                    if (irq_in.sig == SIG_ID_RX) {
                        WARNING("rt_softirq CORE1 sig %d count %d", irq_in.sig, irq_in.count);
                        uint8_t c = core1_read();
                        // TODO
                    }
                    else if (irq_in.sig == SIG_ID_TX)
                    {
                        if (core1_writable()) {
                            core1_write(irq_in.sig);
                        }
                    }
                }
            break;
            case DEV_ID_FLASH:
                //INFO("rt_softirq FLASH sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_TRANSFER_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_flash].transfer();
                    irq_out(DEV_ID_FLASH, res, 0, NULL);
                }
                else if (irq_in.sig == SIG_ID_TRIM_REQ)
                {
                    int res = blockdev[blockdev_id_flash].trim();
                    irq_out(DEV_ID_FLASH, res, 0, NULL);
                }
                else
                {
                    WARNING("rt_softirq FLASH sig %d count %d", irq_in.sig, irq_in.count);
                }
            break;
            case DEV_ID_SD:
                INFO("rt_softirq SD sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_TRANSFER_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_sd].transfer();
                    irq_out(DEV_ID_SD, res, 0, NULL);
                }
                else if (irq_in.sig == SIG_ID_TRIM_REQ)
                {
                    int res = blockdev[blockdev_id_sd].trim();
                    irq_out(DEV_ID_SD, res, 0, NULL);
                }
                else
                {
                    WARNING("rt_softirq SD sig %d count %d", irq_in.sig, irq_in.count);
                }
            break;
            case DEV_ID_USB_VEND0:
                INFO("rt_softirq VEND0 sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_TRANSFER_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_usb_vend0].transfer();
                    irq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else if (irq_in.sig == SIG_ID_TRIM_REQ)
                {
                    int res = blockdev[blockdev_id_usb_vend0].trim();
                    irq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else
                {
                    WARNING("rt_softirq USB sig %d count %d", irq_in.sig, irq_in.count);
                }
            break;
            case DEV_ID_STDIO:
                //INFO("rt_softirq STDIO sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_RX) {
                    stdio_byte = stdio_select_read();
                    stdio_irq_done = true;
                }
                else if (irq_in.sig == SIG_ID_TX)
                {
                    if (stdio_byte == '\n')
                        stdio_select_write('\r');
                    stdio_select_write(stdio_byte);
                    stdio_irq_done = true;
                }
            break;
            case DEV_ID_TTY1:
                //INFO("rt_softirq TTY1 sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_RX) {
                    tty1_byte = tty1_select_read();
                    tty1_irq_done = true;
                }
                else if (irq_in.sig == SIG_ID_TX)
                {
                    if (tty1_byte == '\n')
                        tty1_select_write('\r');
                    tty1_select_write(tty1_byte);
                    tty1_irq_done = true;
                }
            break;
            case DEV_ID_TTY2:
                //INFO("rt_softirq TTY2 sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_RX) {
                    tty2_byte = tty2_select_read();
                    tty2_irq_done = true;
                }
                else if (irq_in.sig == SIG_ID_TX)
                {
                    if (tty2_byte == '\n')
                        tty2_select_write('\r');
                    tty2_select_write(tty2_byte);
                    tty2_irq_done = true;
                }
            break;
            case DEV_ID_TTY3:
                //INFO("rt_softirq TTY3 sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_RX) {
                    tty3_byte = usb_cdc2_read();
                    tty3_irq_done = true;
                }
                else if (irq_in.sig == SIG_ID_TX )
                {
                    if (tty3_byte == '\n')
                        usb_cdc2_write('\r');
                    usb_cdc2_write(tty3_byte);
                    tty3_irq_done = true;
                }
            break;
            case DEV_ID_TTY4:
                //INFO("rt_softirq TTY4 sig %d count %d", irq_in.sig, irq_in.count);
                if (irq_in.sig == SIG_ID_RX) {
                    tty4_byte = usb_cdc2_read();
                    tty4_irq_done = true;
                }
                else if (irq_in.sig == SIG_ID_TX )
                {
                    if (tty4_byte == '\n')
                        usb_cdc3_write('\r');
                    usb_cdc3_write(tty4_byte);
                    tty4_irq_done = true;
                }
            break;
            default:
                ERR("rt_softirq unknown irq sig %d count %d", irq_in.sig, irq_in.count);
            break;
        }
        clear_softirq(&irq_in);
    }
	return true;
}

void softirq_init(void) {
    queue_init(&softirq_in_q, sizeof(softirq_t), UINT8_MAX);
	queue_init(&softirq_out_q, sizeof(softirq_t), UINT8_MAX);

	add_repeating_timer_us(((1000000 / TICKSPERSEC)/2)+1, rt_softirq, NULL, &softirq_timer);
}
