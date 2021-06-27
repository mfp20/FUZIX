#ifndef _PLATFORM_DOT_H
#define _PLATFORM_DOT_H

#include "picosdk.h"
#include "config.h"
#include "devvirt.h"

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

extern bool fuzix_ready;

extern void devtty_init(void);
extern void devflash_init(void);
extern void devsd_spi_init(void);
extern void devusb_init(void);

extern void devcore1_init(core1_main_t c1main);
extern bool devcore1_is_readable(void);
extern bool devcore1_is_writable(void);
extern uint8_t devcore1_getc_blocking(void);
extern void devcore1_putc_blocking(uint8_t b);

#endif

