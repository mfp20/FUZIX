#ifndef _SDK_FUZIX_DOT_H
#define _SDK_FUZIX_DOT_H

// softirq
extern bool fuzix_ready;
extern void fuzix_softirq(void);

// ticker
extern void virtual_ticker_init(void);

// chardev
extern void tty_prepare(void);
extern void virtual_stdio_init(void);

// blockdev
extern void virtual_flash_init(void);
extern void virtual_sd_init(void);
extern void virtual_usb_fs_init(void);

#endif
