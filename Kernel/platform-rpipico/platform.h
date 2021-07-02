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
extern void kgetchar(uint8_t c);

// uart
extern void devvirt_uart_stdio(uint8_t id, bool stdio);
extern void devvirt_uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t devvirt_uart0_read(void);
extern void devvirt_uart0_write(uint8_t b);
extern bool devvirt_uart0_writable(void);
extern void devvirt_uart1_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t devvirt_uart1_read(void);
extern void devvirt_uart1_write(uint8_t b);
extern bool devvirt_uart1_writable(void);

// usb
extern void devusb_cdc_stdio(uint8_t id, bool stdio);
extern void devusb_init(uint8_t stdio);

// flash
extern void devflash_init(void);

// spi
extern void devsd_spi_init(void);

// core1
extern bool devvirt_core1_readable(void);
extern bool devvirt_core1_writable(void);
extern uint8_t devvirt_core1_read(void);
extern void devvirt_core1_write(uint8_t b);
extern void devvirt_core1_init(core1_main_t c1main, byte_tx_t rx_cb);

#endif
