#ifndef _SDK_VIRTDEV_FLASH_DOT_H
#define _SDK_VIRTDEV_FLASH_DOT_H

#include "rt.h"

extern uint8_t blockdev_id_flash;

uint32_t flash_init(void *blk_op);

#endif
