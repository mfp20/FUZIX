
#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_blockdev.h"
#include "rt_blockdev_sd.h"

#include <stdio.h>
#include <string.h>

uint8_t blockdev_id_usb_vend0 = 0;

static critical_section_t usb_critical;


//--------------------------------------------------------------------+
// low level
//--------------------------------------------------------------------+


//--------------------------------------------------------------------+
// blockdev
//--------------------------------------------------------------------+

static uint_fast8_t usb_transfer(void) {

	return 1;  // success
}

static int usb_trim(void) {

	return 0;
}

uint32_t usb_vend0_init(void *blk_op) {
	critical_section_init(&usb_critical);

	printf("USB blockdev, ");

	// TODO
    //devsd_spi_init();
	//devsd_init();

	//printf("%dkB physical %dkB logical at 0x%p: ", nand.num_blocks * 4, lba / 2, XIP_NOCACHE_NOALLOC_BASE + FLASH_OFFSET);

	//blockdev_id_sd = blockdev_add(sd_transfer, NULL, sd_trim, lba, blk_op);

	//return lba;
    return 0;
}

/* vim: sw=4 ts=4 et: */

