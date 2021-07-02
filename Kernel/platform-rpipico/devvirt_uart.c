#include "platform.h"

#include "platform_log.h"

// callbacks for received data
static byte_tx_t rx0_cb = NULL;
static byte_tx_t rx1_cb = NULL;

//--------------------------------------------------------------------+
// stdio drivers
//--------------------------------------------------------------------+

static void stdio_uart0_out_chars(const char *buf, int len) {
    uart_write_blocking(uart0, buf, len);
}
static int stdio_uart0_in_chars(char *buf, int len) {
    uart_read_blocking(uart0, buf, len);
    return len;
}
static void stdio_uart1_out_chars(const char *buf, int len) {
    uart_write_blocking(uart1, buf, len);
}
static int stdio_uart1_in_chars(char *buf, int len) {
    uart_read_blocking(uart1, buf, len);
    return len;
}

static stdio_driver_t stdio_uart_driver[2] = {
  {
  .out_chars = stdio_uart0_out_chars,
  .in_chars = stdio_uart0_in_chars,
  .crlf_enabled = true
  },
  {
  .out_chars = stdio_uart1_out_chars,
  .in_chars = stdio_uart1_in_chars,
  .crlf_enabled = true
  }
};

void uart_stdio(uint8_t id, bool stdio) {
  stdio_set_driver_enabled(&stdio_uart_driver[id], stdio);
  if (stdio) {
    LOG_INF("stdio on USB CDC %d", id);
    LOG_EME("emergency log entry");
    LOG_ALE("alert log entry");
    LOG_CRI("critical log entry");
    LOG_ERR("error log entry");
    LOG_WAR("warning log entry");
    LOG_NOT("notice log entry");
    LOG_INF("info log entry");
    LOG_DEB("debug log entry");
    unsigned char data[20] = {32, 1, 24, 56, 102, 5, 78, 92, 200, 0, 32, 1, 24, 56, 102, 5, 78, 92, 200, 0};
    LOG_HEX(data, 20, "hex %s", "log entry");
  } else {
    LOG_INF("UART%d free'd from stdio", id);
  }
}


//--------------------------------------------------------------------+
// fuzix drivers
//--------------------------------------------------------------------+

// isr code
static void on_rx_isr(uint8_t uart_id) {
    uart_inst_t *uart;
    uint8_t dev_id;
    char c;
    byte_tx_t cb;

    // select uart
    if (uart_id) {
        uart = uart1;
        dev_id = DEV_ID_UART1;
        c = uart1_read();
        cb = rx1_cb;
    } else {
        uart = uart0;
        dev_id = DEV_ID_UART0;
        c = uart0_read();
        cb = rx0_cb;
    }

    // route char
    if (fuzix_ready&&queue_is_empty(&devvirt_byte_q)) {
        if (cb) cb(c);
    } else {
        softirq_t irq;
        // get uart byte and evelope for softirq
        if (!mk_byte_irq(&irq, IRQ_ID_BYTE, NULL, dev_id, OP_ID_READ, c)) {
            // TODO out of memory error -> data lost
            return;
        }
        // queue softirq
        if (!queue_try_add(&devvirt_byte_q, &irq)) {
            // TODO queue full error -> lag -> data lost
        }
    }
}
static void on_rx_isr_uart0(void) {
    on_rx_isr(0);
}
static void on_rx_isr_uart1(void) {
    on_rx_isr(1);
}

// uart0
uint8_t uart0_read(void) {
    return uart_getc(uart0);
}

void uart0_write(uint8_t b) {
    uart_putc(uart0, b);
}

bool uart0_writable(void) {
    return uart_is_writable(uart0);
}

void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb) {
    // uart gpios
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);

    // uart device
    uart_init(uart0, baudrate);
    uart_set_translate_crlf(uart0, true);
    uart_set_fifo_enabled(uart0, false);

    // uart irq
    irq_set_exclusive_handler(UART0_IRQ, on_rx_isr_uart0);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);

    //
    rx0_cb = rx_cb;

    // register stdio driver (disabled)
    uart_stdio(0, false);
}

// uart1
uint8_t uart1_read(void) {
    return uart_getc(uart1);
}

void uart1_write(uint8_t b) {
    uart_putc(uart1, b);
}

bool uart1_writable(void) {
    return uart_is_writable(uart1);
}

void uart1_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb) {
    // uart gpios
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);

    // uart device
    uart_init(uart1, baudrate);
    uart_set_translate_crlf(uart1, true);
    uart_set_fifo_enabled(uart1, false);

    // uart irq
    irq_set_exclusive_handler(UART1_IRQ, on_rx_isr_uart1);
    irq_set_enabled(UART1_IRQ, true);
    uart_set_irq_enables(uart1, true, false);

    //
    rx1_cb = rx_cb;

    // register stdio driver (disabled)
    uart_stdio(1, false);
}

/* vim: sw=4 ts=4 et: */
