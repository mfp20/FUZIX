
#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_blockdev.h"
#include "rt_blockdev_usb.h"
#include "rt_usb_mplex.h"

#include <stdio.h>
#include <string.h>

uint8_t blockdev_id_usb_disk1 = 0;
uint8_t blockdev_id_usb_disk2 = 0;
uint8_t blockdev_id_usb_disk3 = 0;

//--------------------------------------------------------------------+
// blockdev
//--------------------------------------------------------------------+

static uint_fast8_t usb_transfer(uint8_t disk_id) {
	uint8_t id = blockdev_id_usb_disk1;
	if (disk_id==2)
		id = blockdev_id_usb_disk2;
	if (disk_id==3)
		id = blockdev_id_usb_disk3;

	if (blockdev[id].op->is_read)
		; // blockdev[id].op->lba, blockdev[id].op->addr
	else
		; // blockdev[id].op->lba, blockdev[id].op->addr

	return 1;  // success
}

static int usb_trim(uint8_t disk_id) {
	return 0;
}

static uint_fast8_t usb_transfer_disk1(void) {
	return usb_transfer(1);
}

static int usb_trim_disk1(void) {
	return usb_trim(1);
}

static uint_fast8_t usb_transfer_disk2(void) {

	return usb_transfer(2);
}

static int usb_trim_disk2(void) {
	return usb_trim(2);
}

static uint_fast8_t usb_transfer_disk3(void) {
	return usb_transfer(3);
}

static int usb_trim_disk3(void) {
	return usb_trim(3);
}

void usb_blockdev_init(void *blk_op, uint32_t *lba1, uint32_t *lba2, uint32_t *lba3) {
	// root disk
	*lba1 = usb_disk_lba_req(1);
	*lba2 = usb_disk_lba_req(2);
	*lba3 = usb_disk_lba_req(3);
	if ((*lba1)&&(*lba2)&&(*lba3)) {
		blockdev_id_usb_disk1 = blockdev_add(usb_transfer_disk1, NULL, usb_trim_disk1, *lba1, blk_op);
		NOTICE("USB external root disk: %dkB logical", *lba1 / 2);
		blockdev_id_usb_disk2 = blockdev_add(usb_transfer_disk2, NULL, usb_trim_disk2, *lba2, blk_op);
		NOTICE("USB external swap disk: %dkB logical", *lba2 / 2);
		blockdev_id_usb_disk3 = blockdev_add(usb_transfer_disk3, NULL, usb_trim_disk3, *lba3, blk_op);
		NOTICE("USB external scratch disk: %dkB logical", *lba3 / 2);
	}
	else
	{
		NOTICE("USB disks not found");
	}
}

/* vim: sw=4 ts=4 et: */
