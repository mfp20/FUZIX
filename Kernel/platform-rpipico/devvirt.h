#ifndef _DEVVIRT_DOT_H
#define _DEVVIRT_DOT_H

#include "picosdk.h"
#include "config.h"

#define IRQ_ID_TICK 0
#define IRQ_ID_BYTEDEV 1
#define IRQ_ID_BLOCKDEV 2

#define OP_ID_READ 0
#define OP_ID_WRITE 1

#define DEV_ID_CORE1 0
#define DEV_ID_FLASH 1
#define DEV_ID_SD 2
#define DEV_ID_USB 3
#define DEV_ID_UART0 4
#define DEV_ID_UART1 5
#define DEV_ID_I2C0 6
#define DEV_ID_I2C1 7
#define DEV_ID_SPI0 8
#define DEV_ID_SPI1 9

typedef struct iop_s {
    uint8_t op;
	uint8_t dev;
	uint8_t *data;
    uint32_t count;
    bool free;
} iop_t;

typedef struct softirq_s {
	uint8_t id;
	void *data;
    bool free;
    bool free_data;
} softirq_t;

extern pico_queue_t devvirt_signal_q;
extern pico_queue_t devvirt_byte_q;
extern pico_queue_t devvirt_block_q;

bool mk_byte_irq(softirq_t *irq, uint8_t irq_id, void *data_addr, uint8_t dev_id, uint8_t op_id, uint8_t byte);
void clear_byte_irq(softirq_t *irq);

bool mk_irq(softirq_t *irq, uint8_t irq_id, iop_t *iop, uint8_t dev_id, uint8_t op_id, void *iop_data, uint32_t count);
void clear_irq(softirq_t *irq);

void devvirt_service(void);

#endif
