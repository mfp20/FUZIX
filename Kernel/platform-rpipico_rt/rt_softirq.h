#ifndef _SDK_VIRTDEV_DOT_H
#define _SDK_VIRTDEV_DOT_H

#include "rt.h"

#define DEV_ID_RESERVED 0
#define DEV_ID_TIMER 1
#define DEV_ID_CORE1 2
#define DEV_ID_FLASH 3
#define DEV_ID_SD 4
#define DEV_ID_USB_CDC0 5
#define DEV_ID_USB_CDC1 6
#define DEV_ID_USB_CDC2 7
#define DEV_ID_USB_VEND0 8
#define DEV_ID_USB_VEND1 9
#define DEV_ID_STDIO 10
#define DEV_ID_TTY1 11
#define DEV_ID_TTY2 12
#define DEV_ID_TTY3 13

#define SIG_ID_TICK 0
#define SIG_ID_RX 1
#define SIG_ID_TX 2
#define SIG_ID_TX_READY 3
#define SIG_ID_TRANSFER_FLASH_REQ 4
#define SIG_ID_TRIM_FLASH_REQ 5
#define SIG_ID_TRANSFER_SD_REQ 4
#define SIG_ID_TRIM_SD_REQ 5
#define SIG_ID_TRANSFER_FS1_REQ 4
#define SIG_ID_TRIM_FS1_REQ 5
#define SIG_ID_TRANSFER_FS2_REQ 6
#define SIG_ID_TRIM_FS2_REQ 7
#define SIG_ID_TRANSFER_FS3_REQ 8
#define SIG_ID_TRIM_FS3_REQ 9
#define SIG_ID_TIMESTAMP_REQ 10

typedef struct softirq_s {
    uint8_t dev;
    uint8_t sig;
	uint32_t count;
	uint8_t *data;
} softirq_t;

extern pico_queue_t softirq_in_q;
extern pico_queue_t softirq_out_q;

void softirq_init(void);
void softirq_out(uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data);
void softirq_in(uint8_t dev_id, uint8_t signal_id, uint32_t count, void *data);
void softirq_free(softirq_t *irq);

#endif
