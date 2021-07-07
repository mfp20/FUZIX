#ifndef _SDK_VIRTDEV_DOT_H
#define _SDK_VIRTDEV_DOT_H

#include "rt.h"

#define DEV_ID_STDIO 0
#define DEV_ID_TIMER 1
#define DEV_ID_CORE1 2
#define DEV_ID_UART0 3
#define DEV_ID_UART1 4
#define DEV_ID_I2C0 5
#define DEV_ID_I2C1 6
#define DEV_ID_SPI0 7
#define DEV_ID_SPI1 8
#define DEV_ID_FLASH 9
#define DEV_ID_SD 10
#define DEV_ID_USB_CDC0 11
#define DEV_ID_USB_CDC1 12
#define DEV_ID_USB_CDC2 13
#define DEV_ID_USB_CDC3 14
#define DEV_ID_USB_VEND0 15
#define DEV_ID_USB_VEND1 16
#define DEV_ID_USB_VEND2 17

#define SIG_ID_TICK 0
#define SIG_ID_RX 1
#define SIG_ID_TX 2
#define SIG_ID_TRANSFER_REQ 3
#define SIG_ID_TRANSFER_READY 4
#define SIG_ID_TRIM_REQ 5
#define SIG_ID_TRIM_READY 6

typedef struct softirq_s {
    uint8_t dev;
    uint8_t sig;
	uint32_t count;
	uint8_t *data;
} softirq_t;

extern pico_queue_t softirq_in_q;
extern pico_queue_t softirq_out_q;

bool mk_softirq(softirq_t *irq, uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data);
void clear_softirq(softirq_t *irq);

void softirq_init(void);

#endif
