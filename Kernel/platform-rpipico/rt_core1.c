#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_core1.h"
#include "rt_fuzix.h"

static byte_tx_t rx_core1_cb = NULL;

// isr code
static void core1_on_rx_isr(void) {	
    uint8_t b = core1_read();

    if (fuzix_ready && queue_is_empty(&softirq_out_q) && rx_core1_cb) {
		rx_core1_cb(b);
    } else {
        // evelope core1 byte for softirq
        softirq_t irq;
        mk_softirq(&irq, DEV_ID_CORE1, b, 0, NULL);
        // queue softirq
        if (!queue_try_add(&softirq_out_q, &irq)) {
            // TODO queue full error -> lag -> data lost
        }
    }

	multicore_fifo_clear_irq();
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
