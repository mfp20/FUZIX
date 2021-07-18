#include "platform.h"

#include <kernel.h>
#include <blkdev.h>

//--------------------------------------------------------------------+
// virtual blockdev drivers
//--------------------------------------------------------------------+

static uint_fast8_t blockdev_signal(uint8_t dev, uint8_t req, bool *flag) {
	// evelope blockdev request for softirq
	softirq_in(dev, req, 0, NULL);

	// wait for response
	while (!(*flag)) {
		fuzix_softirq();
	}
	*flag = false;

	return 1; // success
}

static uint_fast8_t blockdev_flash_signal(uint8_t req) {
	return blockdev_signal(DEV_ID_FLASH, req, &flash_irq_done);
}

static uint_fast8_t blockdev_sd_signal(uint8_t req) {
	blockdev_signal(DEV_ID_SD, req, &sd_irq_done);
}

static uint_fast8_t blockdev_usb_signal(uint8_t req) {
	blockdev_signal(DEV_ID_USB_VEND0, req, &usb_irq_done);
}

static uint_fast8_t virtual_flash_transfer(void) {
	return blockdev_flash_signal(SIG_ID_TRANSFER_FLASH_REQ);
}

static int virtual_flash_trim(void) {
	blockdev_flash_signal(SIG_ID_TRIM_FLASH_REQ);
	return 0;
}

static uint_fast8_t virtual_sd_transfer(void) {
	return blockdev_sd_signal(SIG_ID_TRANSFER_SD_REQ);
}

static int virtual_sd_trim(void) {
	blockdev_sd_signal(SIG_ID_TRIM_SD_REQ);
	return 0;
}

static uint_fast8_t virtual_usb_disk1_transfer(void) {
	return blockdev_usb_signal(SIG_ID_TRANSFER_USB_DISK1_REQ);
}

static int virtual_usb_disk1_trim(void) {
	blockdev_usb_signal(SIG_ID_TRIM_USB_DISK1_REQ);
	return 0;
}

static uint_fast8_t virtual_usb_disk2_transfer(void) {
	return blockdev_usb_signal(SIG_ID_TRANSFER_USB_DISK2_REQ);
}

static int virtual_usb_disk2_trim(void) {
	blockdev_usb_signal(SIG_ID_TRIM_USB_DISK2_REQ);
	return 0;
}

static uint_fast8_t virtual_usb_disk3_transfer(void) {
	return blockdev_usb_signal(SIG_ID_TRANSFER_USB_DISK3_REQ);
}

static int virtual_usb_disk3_trim(void) {
	blockdev_usb_signal(SIG_ID_TRIM_USB_DISK3_REQ);
	return 0;
}

//--------------------------------------------------------------------+
// init
//--------------------------------------------------------------------+

static void fuzix_blkdev_add(transfer_function_t transfer_cb, flush_function_t flush_cb, trim_function_t trim_cb, uint32_t lba) {
	blkdev_t *blk = blkdev_alloc();
	if (!blk)
		return;
	
	blk->transfer = transfer_cb;
	blk->flush = flush_cb;
    #ifdef CONFIG_TRIM
        blk->trim = trim_cb;
    #endif
	blk->drive_lba_count = lba;

	blkdev_scan(blk, 0);
}

void virtual_flash_init(void) {
	// init real device
	uint32_t lba = flash_init(&blk_op);

	// init virtual device
	fuzix_blkdev_add(virtual_flash_transfer, NULL, virtual_flash_trim, lba);
}

void virtual_sd_init(void) {
	// init real device
	uint32_t lba = sd_init(&blk_op);

	// init virtual device
	//fuzix_blkdev_add(virtual_sd_transfer, NULL, virtual_sd_trim, lba);
}

void virtual_usb_disk_init(void) {
	if (usb_vend0_blockdev_enabled) {
		// init real device
		uint32_t lba1, lba2, lba3;
		usb_blockdev_init(&blk_op, &lba1, &lba2, &lba3);

		// init virtual device
		if ((lba1)&&(lba2)&&(lba3)) {
			fuzix_blkdev_add(virtual_usb_disk1_transfer, NULL, virtual_usb_disk1_trim, lba1);
			fuzix_blkdev_add(virtual_usb_disk2_transfer, NULL, virtual_usb_disk2_trim, lba2);
			fuzix_blkdev_add(virtual_usb_disk3_transfer, NULL, virtual_usb_disk3_trim, lba3);
		}
	}
}
