#ifndef _RT_FUZIX_DOT_H
#define _RT_FUZIX_DOT_H

// softirq
extern bool fuzix_ready;
extern void fuzix_softirq(void);

// ticker
extern void virtual_ticker_init(void);

// chardev
extern void chardev_init(void);
extern void fuzix_ttys_prepare(void);
extern void fuzix_tty1_write(uint8_t b);
extern void fuzix_tty2_write(uint8_t b);
extern void fuzix_tty3_write(uint8_t b);

// blockdev
extern void virtual_flash_init(void);
extern void virtual_sd_init(void);
extern void virtual_usb_disk_init(void);

#endif
