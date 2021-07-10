#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_uart.h"
#include "rt_fuzix.h"

#include <tusb.h>

// callback for received data
static byte_tx_t rx0_cb = NULL;

//--------------------------------------------------------------------+
// isr, init, helpers
//--------------------------------------------------------------------+

static void on_uart0_rx_isr(void)
{
	uint8_t b = uart_getc(uart0);

	if (tud_cdc_n_connected(0))
	{
		if (rx0_cb) rx0_cb(b);
	}
	else
	{
		// evelope uart0 byte for softirq
		softirq_out(DEV_ID_TTY1, b, 0, NULL);
	}
}

void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb)
{
	// uart gpios
	gpio_set_function(tx_pin, GPIO_FUNC_UART);
	gpio_set_function(rx_pin, GPIO_FUNC_UART);

	// uart device
	uart_init(uart0, baudrate);
	uart_set_translate_crlf(uart0, true);
	uart_set_fifo_enabled(uart0, false);

	// uart irq
	irq_set_exclusive_handler(UART0_IRQ, on_uart0_rx_isr);
	irq_set_enabled(UART0_IRQ, true);
	uart_set_irq_enables(uart0, true, false);

	//
	rx0_cb = rx_cb;
}

void uart0_set_cb(byte_tx_t rx_cb) {
	rx0_cb = rx_cb;
}

/* vim: sw=4 ts=4 et: */
