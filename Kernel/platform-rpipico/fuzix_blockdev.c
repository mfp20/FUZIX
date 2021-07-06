#include "platform.h"

#include <kernel.h>
#include <blkdev.h>

static uint_fast8_t blockdev_signal(uint8_t dev, uint8_t req) {
	// evelope blockdev reqeust for softirq
	softirq_t irq;
	mk_softirq(&irq, dev, req, 0, NULL);

	// queue request
	if (!queue_try_add(&softirq_in_q, &irq))
	{
		// TODO queue full error -> lag
		return 0;
	}

	// wait for response
	while (!flash_irq_done) {
		// handle cpu to other processes while waiting
		//stdio_printf("waiting for flash\n");
		switchout();
	}
    //stdio_printf("FLASH ok\n");
	flash_irq_done = false;

	return 1; // success
}

static uint_fast8_t blockdev_flash_signal(uint8_t req) {
	return blockdev_signal( DEV_ID_FLASH, req);
}

static uint_fast8_t blockdev_sd_signal(uint8_t req) {
	blockdev_signal( DEV_ID_SD, req);
}

static uint_fast8_t blockdev_usb_signal(uint8_t req) {
	blockdev_signal( DEV_ID_USB_VEND0, req);
}

static uint_fast8_t virtual_flash_transfer(void) {
	return blockdev_flash_signal(SIG_ID_TRANSFER_REQ);
}

static int virtual_flash_trim(void) {
	blockdev_flash_signal(SIG_ID_TRIM_REQ);
	return 0;
}

static uint_fast8_t virtual_sd_transfer(void) {
	return blockdev_sd_signal(SIG_ID_TRANSFER_REQ);
}

static int virtual_sd_trim(void) {
	blockdev_sd_signal(SIG_ID_TRIM_REQ);
	return 0;
}

static uint_fast8_t virtual_usb_transfer(void) {
	return blockdev_usb_signal(SIG_ID_TRANSFER_REQ);
}

static int virtual_usb_trim(void) {
	blockdev_usb_signal(SIG_ID_TRIM_REQ);
	return 0;
}

static void blkdev_add(transfer_function_t transfer_cb, flush_function_t flush_cb, trim_function_t trim_cb, uint32_t lba) {
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
	blkdev_add(virtual_flash_transfer, NULL, virtual_flash_trim, lba);
}

void virtual_sd_init(void) {
	// init real device
	// TODO uint32_t lba = sd_init(&blk_op);

	// init virtual device
	//blkdev_add(virtual_sd_transfer, NULL, virtual_sd_trim, lba);
}

void virtual_usb_init(void) {
	// init real device
	// TODO uint32_t lba = usb_init(&blk_op);

	// init virtual device
	//blkdev_add(virtual_usb_transfer, NULL, virtual_usb_trim, lba);
}
