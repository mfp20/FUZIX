#include "rt_blockdev.h"

#include <stdlib.h>

// all available blockdevs
blockdev_t *blockdev;
uint8_t blockdev_no = 0;
uint8_t blockdev_flash_id = 0;
uint8_t blockdev_sd_id = 0;

uint8_t blockdev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w) {
    blockdev_no++;
    blockdev = realloc(blockdev, sizeof(blockdev_t)*blockdev_no);
    blockdev[blockdev_no].rx = r;
    blockdev[blockdev_no].tx = t;
    blockdev[blockdev_no].ready = w;

    return blockdev_no-1;
}

void blockdev_set_flash(uint8_t blockdev_id) {
    blockdev_flash_id = blockdev_id;
}

void blockdev_set_sd(uint8_t blockdev_id) {
    blockdev_sd_id = blockdev_id;
}

/* vim: sw=4 ts=4 et: */
