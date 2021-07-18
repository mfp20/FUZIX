#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_chardev.h"
#include "rt_core1.h"
#include "rt_blockdev.h"
#include "rt_blockdev_flash.h"
#include "rt_blockdev_sd.h"
#include "rt_blockdev_usb.h"
#include "rt_usb.h"
#include "rt_usb_mplex.h"

#include <stdlib.h>

pico_queue_t softirq_in_q;
pico_queue_t softirq_out_q;

bool stdio_irq_done = false;
bool tty1_irq_done = false;
bool tty2_irq_done = false;
bool tty3_irq_done = false;
bool flash_irq_done = false;
bool sd_irq_done = false;
bool usb_irq_done = false;

static repeating_timer_t softirq_timer;

static bool softirq_alloc(softirq_t *irq, uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data) {
    irq->dev = dev_id;
    irq->sig = signal_id;
    irq->count = count;
    if (count) {
        irq->data = data;
    }
    return true;
}

void softirq_out(uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data) {
    softirq_t irq;
    softirq_alloc(&irq, dev_id, signal_id, count, data);
    while (!queue_try_add(&softirq_out_q, &irq)) ; // TODO queue full error -> lag
}

void softirq_in(uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data) {
    softirq_t irq;
    softirq_alloc(&irq, dev_id, signal_id, count, data);
    while (!queue_try_add(&softirq_in_q, &irq)) ; // TODO queue full error -> lag
}

void softirq_free(softirq_t *irq) {
    if (irq->count)
        free(irq->data);
}

// process all data from fuzix's virtual devices to real hardware
static bool rt_softirq(repeating_timer_t *rt)
{
    softirq_t softirq_in;
	while (!queue_is_empty(&softirq_in_q)) {
        queue_remove_blocking(&softirq_in_q, &softirq_in);
        switch (softirq_in.dev) {
            case DEV_ID_TICKER:
                WARN("rt_softirq TIMER sig %d count %d", softirq_in.sig, softirq_in.count);
            break;
            case DEV_ID_CORE1:
                INFO("rt_softirq CORE1 sig %d count %d", softirq_in.sig, softirq_in.count);
                if (softirq_in.count) {
                    WARN("rt_softirq CORE1 sig %d count %d", softirq_in.sig, softirq_in.count);
                    // TODO
                }
                else
                {
                    if (softirq_in.sig == SIG_ID_RX) {
                        WARN("rt_softirq CORE1 sig %d count %d", softirq_in.sig, softirq_in.count);
                        uint8_t c = core1_read();
                        // TODO
                    }
                    else if (softirq_in.sig == SIG_ID_TX)
                    {
                        if (core1_writable()) {
                            core1_write(softirq_in.sig);
                        }
                    }
                }
            break;
            case DEV_ID_FLASH:
                //INFO("rt_softirq FLASH sig %d count %d", softirq_in.sig, softirq_in.count);
                if (softirq_in.sig == SIG_ID_TRANSFER_FLASH_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_flash].transfer();
                    softirq_out(DEV_ID_FLASH, res, 0, NULL);
                }
                else if (softirq_in.sig == SIG_ID_TRIM_FLASH_REQ)
                {
                    int res = blockdev[blockdev_id_flash].trim();
                    softirq_out(DEV_ID_FLASH, res, 0, NULL);
                }
                else
                {
                    WARN("rt_softirq FLASH sig %d count %d", softirq_in.sig, softirq_in.count);
                }
            break;
            case DEV_ID_SD:
                INFO("rt_softirq SD sig %d count %d", softirq_in.sig, softirq_in.count);
                if (softirq_in.sig == SIG_ID_TRANSFER_SD_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_sd].transfer();
                    softirq_out(DEV_ID_SD, res, 0, NULL);
                }
                else if (softirq_in.sig == SIG_ID_TRIM_SD_REQ)
                {
                    int res = blockdev[blockdev_id_sd].trim();
                    softirq_out(DEV_ID_SD, res, 0, NULL);
                }
                else
                {
                    WARN("rt_softirq SD sig %d count %d", softirq_in.sig, softirq_in.count);
                }
            break;
            case DEV_ID_USB_VEND0:
                INFO("rt_softirq VEND0 sig %d count %d", softirq_in.sig, softirq_in.count);
                if (softirq_in.sig == SIG_ID_DATETIME_REQ) {
                    datetime_t *datetime = malloc(sizeof(datetime_t));
                    int res = usb_datetime_req(datetime) ? 1 : 0;
                    softirq_out(DEV_ID_USB_VEND0, res, 4, datetime);
                }
                else if (softirq_in.sig == SIG_ID_TRANSFER_USB_DISK1_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_usb_disk1].transfer();
                    softirq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else if (softirq_in.sig == SIG_ID_TRIM_USB_DISK1_REQ)
                {
                    int res = blockdev[blockdev_id_usb_disk1].trim();
                    softirq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else if (softirq_in.sig == SIG_ID_TRANSFER_USB_DISK2_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_usb_disk2].transfer();
                    softirq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else if (softirq_in.sig == SIG_ID_TRIM_USB_DISK2_REQ)
                {
                    int res = blockdev[blockdev_id_usb_disk2].trim();
                    softirq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else if (softirq_in.sig == SIG_ID_TRANSFER_USB_DISK3_REQ) {
                    uint_fast8_t res = blockdev[blockdev_id_usb_disk3].transfer();
                    softirq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else if (softirq_in.sig == SIG_ID_TRIM_USB_DISK3_REQ)
                {
                    int res = blockdev[blockdev_id_usb_disk3].trim();
                    softirq_out(DEV_ID_USB_VEND0, res, 0, NULL);
                }
                else
                {
                    WARN("rt_softirq USB sig %d count %d", softirq_in.sig, softirq_in.count);
                }
            break;
            case DEV_ID_STDIO:
                if (softirq_in.sig == SIG_ID_RX) {
                    stdio_byte = rt_select_read();
                    stdio_irq_done = true;
                }
                else if (softirq_in.sig == SIG_ID_TX)
                {
                    if (softirq_in.count) {
                        WARN("rt_softirq DEV_ID_STDIO, count > 0, NOT IMPLEMENTED");
                    }
                    else
                    {
                        if (stdio_byte == '\n')
                            stdio_kputchar('\r');
                        stdio_kputchar(stdio_byte);
                        stdio_irq_done = true;
                    }
                }
            break;
            case DEV_ID_TTY1: // 
                if (softirq_in.sig == SIG_ID_RX) {
                    tty1_byte = fuzix_select_read();
                    tty1_irq_done = true;
                }
                else if (softirq_in.sig == SIG_ID_TX)
                {
                    if (tty1_byte == '\n')
                        fuzix_select_write('\r');
                    fuzix_select_write(tty1_byte);
                    tty1_irq_done = true;
                }
            break;
            case DEV_ID_TTY2:
                if (softirq_in.sig == SIG_ID_RX) {
                    tty2_byte = rt_select_read();
                    tty2_irq_done = true;
                }
                else if (softirq_in.sig == SIG_ID_TX)
                {
                    if (tty2_byte == '\n')
                        rt_select_write('\r');
                    rt_select_write(tty2_byte);
                    tty2_irq_done = true;
                }
            break;
            case DEV_ID_TTY3:
                if (softirq_in.sig == SIG_ID_RX) {
                    tty3_byte = tty3_select_read();
                    tty3_irq_done = true;
                }
                else if (softirq_in.sig == SIG_ID_TX )
                {
                    if (tty3_byte == '\n')
                        tty3_select_write('\r');
                    tty3_select_write(tty3_byte);
                    tty3_irq_done = true;
                }
            break;
            default:
                ERR("rt_softirq unknown irq sig %d count %d", softirq_in.sig, softirq_in.count);
            break;
        }
        softirq_free(&softirq_in);
    }
	return true;
}

void softirq_init(void) {
    queue_init(&softirq_in_q, sizeof(softirq_t), UINT8_MAX);
	queue_init(&softirq_out_q, sizeof(softirq_t), UINT8_MAX);

    // uses lowest prio alarm pool
	alarm_pool_add_repeating_timer_us(alarm_pool[ALARM_POOL_BE], ((1000000 / TICKSPERSEC)/2)+1, rt_softirq, NULL, &softirq_timer);
}
