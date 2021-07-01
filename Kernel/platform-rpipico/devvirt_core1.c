#include "platform.h"

static byte_tx_t tx = NULL;

bool devvirt_core1_readable(void) {
	return multicore_fifo_rvalid();
}

bool devvirt_core1_writable(void) {
	return multicore_fifo_wready();
}

uint8_t devvirt_core1_read(void) {
	return multicore_fifo_pop_blocking();
}

void devvirt_core1_write(uint8_t b) {
	multicore_fifo_push_blocking(b);
}

static void core1_on_rx_isr(void) {
	char c = devvirt_core1_read();
	
    if (fuzix_ready&&queue_is_empty(&devvirt_byte_q)) {
		if (tx) tx(c);
    } else {
        softirq_t irq;
        // get core1 byte and evelope for softirq
        if (!mk_byte_irq(&irq, IRQ_ID_BYTE, NULL, DEV_ID_CORE1, OP_ID_READ, c)) {
            // TODO out of memory error
            return;
        }
        // queue softirq
        if (!queue_try_add(&devvirt_byte_q, &irq)) {
            // TODO queue full error -> lag -> data lost
        }
    }

	multicore_fifo_clear_irq();
}

void devvirt_core1_init(core1_main_t c1main, byte_tx_t rx_cb) {
	multicore_launch_core1(c1main);

	// rx irq
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_on_rx_isr);
    irq_set_enabled(SIO_IRQ_PROC1, true);

    //
    tx = rx_cb;
}
