#ifndef _DEVVIRT_DOT_H
#define _DEVVIRT_DOT_H

#include "picosdk.h"
#include "config.h"

#define IRQ_ID_SIGNAL 0
#define IRQ_ID_BYTE 1
#define IRQ_ID_BLOCK 2

#define DEV_ID_TIMER 0
#define DEV_ID_CORE1 1
#define DEV_ID_UART0 2
#define DEV_ID_UART1 3
#define DEV_ID_I2C0 4
#define DEV_ID_I2C1 5
#define DEV_ID_SPI0 6
#define DEV_ID_SPI1 7
#define DEV_ID_FLASH 8
#define DEV_ID_SD 9
#define DEV_ID_USB_CDC0 10
#define DEV_ID_USB_CDC1 11
#define DEV_ID_USB_CDC2 12
#define DEV_ID_USB_CDC3 13
#define DEV_ID_USB_VEND0 14
#define DEV_ID_USB_VEND1 15
#define DEV_ID_USB_VEND2 16

#define OP_ID_READ 0
#define OP_ID_WRITE 1

typedef struct softirq_s {
	uint8_t id;
	void *data;
    bool free;
    bool free_data;
} softirq_t;

typedef struct iop_s {
    uint8_t op;
	uint8_t dev;
	uint8_t *data;
    uint32_t count;
    bool free;
} iop_t;

extern pico_queue_t devvirt_signal_q;
extern pico_queue_t devvirt_byte_q;
extern pico_queue_t devvirt_block_q;

bool mk_byte_irq(softirq_t *irq, uint8_t irq_id, void *data_addr, uint8_t dev_id, uint8_t op_id, uint8_t byte);
void clear_byte_irq(softirq_t *irq);

bool mk_irq(softirq_t *irq, uint8_t irq_id, iop_t *iop, uint8_t dev_id, uint8_t op_id, void *iop_data, uint32_t count);
void clear_irq(softirq_t *irq);

void devvirt_service_signal(void);
void devvirt_service_byte(void);
void devvirt_service_block(void);
void devvirt_service_quick(void);
void devvirt_service_flush(void);

#endif
