#include "platform.h"

#include <stdlib.h>

// all available chardevs
chardev_t *chardev;
uint8_t chardev_no = 0;

// chardevs for console and log
uint8_t chardev_console_id = 0;
uint8_t chardev_log_id = 0;

uint8_t chardev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w) {
    if (chardev_no)
        chardev = realloc(chardev, sizeof(chardev_t)*(chardev_no+1));
    else
        chardev = malloc(sizeof(chardev_t));
    chardev[chardev_no].rx = r;
    chardev[chardev_no].tx = t;
    chardev[chardev_no].ready = w;
    chardev_no++;

    return chardev_no-1;
}

void chardev_set_console(uint8_t chardev_id) {
    chardev_console_id = chardev_id;
}

void chardev_set_log(uint8_t chardev_id) {
    chardev_log_id = chardev_id;
}

/* vim: sw=4 ts=4 et: */
