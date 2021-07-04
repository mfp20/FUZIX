#ifndef _SDK_BLOCKDEV_DOT_H
#define _SDK_BLOCKDEV_DOT_H

#include "rt.h"

// clone of Fuzix's struct blkparam
typedef struct blkparam_s {
    /* do not change the order */
    uint8_t *addr;                          /* address for transfer buffer */
    uint8_t is_user;                        /* 0: kernel 1: user 2: swap */
    uint8_t swap_page;                      /* page to pass to map_swap */
    void *blkdev;                       	/* active block device */
    uint32_t lba;                           /* LBA for first sectors to transfer */
    uint16_t nblock;                        /* number of sectors to transfer */
    bool is_read;                           /* true: read sectors, false: write sectors */
} blkparam_t;

typedef struct blockdev_s
{
	uint_fast8_t (*transfer)(void);
	int (*flush)(void);
	int (*trim)(void);
	uint32_t lba;
	blkparam_t *op;
} blockdev_t;

extern blockdev_t *blockdev;

uint8_t blockdev_add(uint_fast8_t (*transfer)(void), int (*flush)(void), int (*trim)(void), uint32_t lba, void *op);

#endif
