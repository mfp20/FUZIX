#include "rt_log.h"
#include "rt_chardev.h"

#include <stdlib.h>

// all available chardevs
chardev_t *chardev = NULL;
static uint8_t chardev_no = 0;

uint8_t chardev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w)
{
	if (chardev_no)
		chardev = realloc(chardev, sizeof(chardev_t) * (chardev_no + 1));
	else
		chardev = malloc(sizeof(chardev_t));
	chardev[chardev_no].rx = r;
	chardev[chardev_no].tx = t;
	chardev[chardev_no].ready = w;
	chardev_no++;

	return chardev_no - 1;
}

void chardev_mod(uint8_t chardev_id, byte_rx_t r, byte_tx_t t, byte_ready_t w)
{
	chardev[chardev_id].rx = r;
	chardev[chardev_id].tx = t;
	chardev[chardev_id].ready = w;
}

/* vim: sw=4 ts=4 et: */
