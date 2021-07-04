#ifndef _SDK_VIRTDEV_USB_DOT_H
#define _SDK_VIRTDEV_USB_DOT_H

#include "rt.h"

void usb_init(void);
void usb_cdc_stdio(uint8_t id, bool stdio);

uint8_t usb_cdc0_read(void);
void usb_cdc0_write(uint8_t b);
bool usb_cdc0_writable(void);

uint8_t usb_cdc1_read(void);
void usb_cdc1_write(uint8_t b);
bool usb_cdc1_writable(void);

#endif
