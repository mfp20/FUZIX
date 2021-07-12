#ifndef _RT_BLOCKDEV_USB_DOT_H
#define _RT_BLOCKDEV_USB_DOT_H

#include "rt.h"

extern uint8_t blockdev_id_usb_vend0_fs1;
extern uint8_t blockdev_id_usb_vend0_fs2;
extern uint8_t blockdev_id_usb_vend0_fs3;

void usb_vend0_init(void *blk_op, uint32_t *lba1, uint32_t *lba2, uint32_t *lba3);

#endif
