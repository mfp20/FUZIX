#include "platform.h"
#include "softirq.h"

#include <kernel.h>
#include <kdata.h>
#include <printf.h>
#include <vt.h>
#include <tty.h>


static uint8_t ttybuf[TTYSIZ];

struct s_queue ttyinq[NUM_DEV_TTY+1] = { /* ttyinq[0] is never used */
	{ 0,         0,         0,         0,      0, 0        },
	{ ttybuf,    ttybuf,    ttybuf,    TTYSIZ, 0, TTYSIZ/2 },
};

tcflag_t termios_mask[NUM_DEV_TTY+1] = { 0, _CSYS };

// Output for the system console (kprintf etc)
void kputchar(uint_fast8_t c) {
    if (c=='\n')
        uart_putc(uart0, '\r');
    uart_putc(uart0, c);
}

void tty_putc(uint_fast8_t minor, uint_fast8_t c) {
	kputchar(c);
}

ttyready_t tty_writeready(uint_fast8_t minor) {
    return uart_is_writable(uart0) ? TTY_READY_NOW : TTY_READY_SOON;
}

int tty_carrier(uint_fast8_t minor) {
    return 1;
}

void tty_sleeping(uint_fast8_t minor) {}
void tty_data_consumed(uint_fast8_t minor) {}
void tty_setup(uint_fast8_t minor, uint_fast8_t flags) {}

static void tty_isr(void) {
    uarg_t irq = IRQ_ID_BOOT_TTY;
    if (queue_is_full(&uart0_q)) {
        // TODO error buffer full
    } else {
        if (!queue_try_add(&fuzix_softirq_q, &irq)) {
            // TODO lag error
        } else {
            while (uart_is_readable(uart0)) {
                uint8_t b = uart_getc(uart0);
                queue_try_add(&uart0_q, &b);
            }
        } 
    }
}

void devtty_init(void) {
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    uart_init(uart0, 115200);
    uart_set_translate_crlf(uart0, true);
    uart_set_fifo_enabled(uart0, false);

    irq_set_exclusive_handler(UART0_IRQ, tty_isr);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);
}

/* vim: sw=4 ts=4 et: */

