#ifndef _SDK_FUZIX_DOT_H
#define _SDK_FUZIX_DOT_H

#include "rt.h"

// tty
extern void tty_prepare(void);
extern void tty0_inproc(uint8_t c);
extern void tty1_inproc(uint8_t c);

// SPIs
extern void devsd_spi_init(void);

// flash
extern void devflash_init(void);

#endif
