#include "platform.h"

static byte_tx_t rx_core1_cb = NULL;

// isr code
static void core1_on_rx_isr(void) {
	char c = core1_read();
	
    if (fuzix_ready&&queue_is_empty(&devvirt_byte_q)) {
		if (rx_core1_cb) rx_core1_cb(c);
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

bool core1_readable(void) {
	return multicore_fifo_rvalid();
}

bool core1_writable(void) {
	return multicore_fifo_wready();
}

uint8_t core1_read(void) {
	return multicore_fifo_pop_blocking();
}

void core1_write(uint8_t b) {
	multicore_fifo_push_blocking(b);
}

void core1_init(core1_main_t c1main, byte_tx_t rx_cb) {
	multicore_launch_core1(c1main);

	// rx irq
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_on_rx_isr);
    irq_set_enabled(SIO_IRQ_PROC1, true);

    //
    rx_core1_cb = rx_cb;
}
