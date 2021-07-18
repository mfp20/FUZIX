#include "rt_log.h"
#include "rt_blockdev.h"

#include <stdlib.h>

// all available blockdevs
blockdev_t *blockdev;
uint8_t blockdev_no = 0;

uint8_t blockdev_add(uint_fast8_t (*transfer)(void), int (*flush)(void), int (*trim)(void), uint32_t lba, void *op) {
	if (blockdev_no)
		blockdev = realloc(blockdev, sizeof(blockdev_t) * (blockdev_no + 1));
	else
		blockdev = malloc(sizeof(blockdev_t));

    blockdev[blockdev_no].transfer = transfer;
    blockdev[blockdev_no].flush = flush;
    blockdev[blockdev_no].trim = trim;
    blockdev[blockdev_no].lba = lba;
    blockdev[blockdev_no].op = (blkparam_t *)op;
	blockdev_no++;

	return blockdev_no - 1;
}

/* vim: sw=4 ts=4 et: */
