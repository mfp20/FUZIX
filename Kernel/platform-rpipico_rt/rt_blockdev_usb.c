
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

	NOTICE("USB external filesystem: init");

	uint32_t lba = 0;
	
	if (lba) {
		blockdev_id_usb_vend0 = blockdev_add(usb_transfer, NULL, usb_trim, lba, blk_op);
		NOTICE("USB external filesystem: %dkB logical", lba / 2);
	}
	else
	{
		NOTICE("USB external filesystem: not found on boot");
	}
	WARN("USB external filesystem: NOT IMPLEMENTED");
	//return lba;
    return 0;
}

/* vim: sw=4 ts=4 et: */

