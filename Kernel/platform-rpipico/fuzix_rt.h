#ifndef _FUZIX_RT_DOT_H
#define _FUZIX_RT_DOT_H

#include <version.h>
#include <kernel.h>
#include <blkdev.h>

// virtual devices softirq dispatchers
extern void fuzix_softirq(void);

// ticker
extern void ticker_init(void);

// blockdev
extern void virtual_flash_init(void);
extern void virtual_sd_init(void);
extern void virtual_usb_init(void);

#endif
