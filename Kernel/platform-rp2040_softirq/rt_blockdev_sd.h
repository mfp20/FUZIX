#ifndef _SDK_VIRTDEV_SD_DOT_H
#define _SDK_VIRTDEV_SD_DOT_H

#include "rt.h"

extern uint8_t blockdev_id_sd;

uint32_t sd_init(void *blk_op);

#endif
