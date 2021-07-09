#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_uart.h"
#include "rt_fuzix.h"

#include <tusb.h>

// callbacks for received data
static byte_tx_t rx0_cb = NULL;
static byte_tx_t rx1_cb = NULL;

//--------------------------------------------------------------------+
// stdio drivers
//--------------------------------------------------------------------+

static void stdio_uart0_out_chars(const char *buf, int len)
{
	uart_write_blocking(uart0, buf, len);
}
static int stdio_uart0_in_chars(char *buf, int len)
{
	uart_read_blocking(uart0, buf, len);
	return len;
}
static void stdio_uart1_out_chars(const char *buf, int len)
{
	uart_write_blocking(uart1, buf, len);
}
static int stdio_uart1_in_chars(char *buf, int len)
{
	uart_read_blocking(uart1, buf, len);
	return len;
}

static stdio_driver_t stdio_uart_driver[2] = {
	{.out_chars = stdio_uart0_out_chars,
	 .in_chars = stdio_uart0_in_chars,
	 .crlf_enabled = true},
	{.out_chars = stdio_uart1_out_chars,
	 .in_chars = stdio_uart1_in_chars,
	 .crlf_enabled = true}};

void uart_stdio(uint8_t id, bool stdio, bool test)
{
	stdio_set_driver_enabled(&stdio_uart_driver[id], stdio);
	if (stdio)
	{
		if (test&&(LOG_COLOR)) {
			log_test_color();
			INFO("stdio on UART%d", id);
		}
	}
	else
	{
		INFO("UART%d free'd from stdio", id);
	}
}

//--------------------------------------------------------------------+
// chardev drivers
//--------------------------------------------------------------------+

// uart0
uint8_t uart0_read(void)
{
	return uart_getc(uart0);
}

void uart0_write(uint8_t b)
{
	uart_putc(uart0, b);
}

bool uart0_writable(void)
{
	return uart_is_writable(uart0);
}

// uart1
uint8_t uart1_read(void)
{
	return uart_getc(uart1);
}

void uart1_write(uint8_t b)
{
	uart_putc(uart1, b);
}

bool uart1_writable(void)
{
	return uart_is_writable(uart1);
}

//--------------------------------------------------------------------+
// isr, init, helpers
//--------------------------------------------------------------------+

// isr code
static void on_uart0_rx_isr(void)
{
	uint8_t b = uart0_read();

	if (tud_cdc_n_connected(0))
	{
		if (rx0_cb) rx0_cb(b);
	}
	else
	{
		// evelope uart0 byte for softirq
		irq_out(DEV_ID_TTY1, b, 0, NULL);
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
