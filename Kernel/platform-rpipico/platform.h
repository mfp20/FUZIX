#ifndef _PLATFORM_DOT_H
#define _PLATFORM_DOT_H

#include "picosdk.h"
#include "config.h"
#include "devvirt.h"
#include "devpower.h"

#include "tusb_config.h"

#define FLASH_OFFSET (96*1024)

struct svc_frame {
	uint32_t r12;
	uint32_t pc;
	uint32_t lr;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
};

struct exception_frame {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};

struct extended_exception_frame {
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t cause;
	uint32_t sp;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};

//
typedef uint8_t (*byte_rx_t)(void);
typedef void (*byte_tx_t)(uint8_t);
typedef bool (*byte_ready_t)(void);

// signal fuzix is in 'ei'/'irqrestore', ie: core code can be used freely
extern bool fuzix_ready;

// log
extern void log_set_level(uint8_t level);

// tty
extern void devtty_init(void);
extern void devtty_bind(uint8_t n, byte_rx_t r, byte_tx_t t, byte_ready_t w);
extern void devtty_set_console(uint8_t con);
extern void tty0_putc(uint8_t c);
extern void tty1_putc(uint8_t c);

// core1
extern uint8_t core1_read(void);
extern void core1_write(uint8_t b);
extern bool core1_writable(void);
extern bool core1_readable(void);
extern void core1_init(core1_main_t c1main, byte_tx_t rx_cb);

// uart
extern void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t uart0_read(void);
extern void uart0_write(uint8_t b);
extern bool uart0_writable(void);
extern void uart1_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t uart1_read(void);
extern void uart1_write(uint8_t b);
extern bool uart1_writable(void);
extern void uart_stdio(uint8_t id, bool stdio);

// spi
extern void devsd_spi_init(void);

// flash
extern void devflash_init(void);

// usb
extern uint8_t usb_cdc0_read(void);
extern void usb_cdc0_write(uint8_t b);
extern bool usb_cdc0_writable(void);
extern void usb_init(uint8_t stdio);
extern void usb_cdc_stdio(uint8_t id, bool stdio);

#endif
