
#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_blockdev.h"
#include "rt_flash.h"

#include <lib/dhara/map.h>
#include <lib/dhara/nand.h>

#include <stdio.h>
#include <string.h>

uint8_t blockdev_id_flash = 0;
bool flash_irq_done = false;

static critical_section_t flash_critical;
static const struct dhara_nand nand = {
	.log2_page_size = 9,	// 512 bytes
	.log2_ppb = 12 - 9,		// 4096 bytes
	.num_blocks = (PICO_FLASH_SIZE_BYTES - FLASH_OFFSET) / 4096,
};
static struct dhara_map dhara;
static uint8_t journal_buf[512];
static uint8_t tmp_buf[512];

//--------------------------------------------------------------------+
// low level
//--------------------------------------------------------------------+

int dhara_nand_erase(const struct dhara_nand *n, dhara_block_t b, dhara_error_t *err) {
	critical_section_enter_blocking(&flash_critical);
    flash_range_erase(FLASH_OFFSET + (b*4096), 4096);
	critical_section_exit(&flash_critical);
	if (err)
		*err = DHARA_E_NONE;
	return 0;
}

int dhara_nand_prog(const struct dhara_nand *n, dhara_page_t p, const uint8_t *data, dhara_error_t *err) {
	critical_section_enter_blocking(&flash_critical);
    flash_range_program(FLASH_OFFSET + (p*512), data, 512);
	critical_section_exit(&flash_critical);
	if (err)
		*err = DHARA_E_NONE;
	return 0;
}

int dhara_nand_read(const struct dhara_nand *n, dhara_page_t p, size_t offset, size_t length, uint8_t *data, dhara_error_t *err) {
	critical_section_enter_blocking(&flash_critical);
    memcpy(data, (uint8_t*)XIP_NOCACHE_NOALLOC_BASE + FLASH_OFFSET + (p*512) + offset, length);
	critical_section_exit(&flash_critical);
	if (err)
		*err = DHARA_E_NONE;
	return 0;
}

int dhara_nand_is_bad(const struct dhara_nand *n, dhara_block_t b) {
	return 0;
}

void dhara_nand_mark_bad(const struct dhara_nand *n, dhara_block_t b) {}

int dhara_nand_is_free(const struct dhara_nand *n, dhara_page_t p) {
	dhara_error_t err = DHARA_E_NONE;

	dhara_nand_read(&nand, p, 0, 512, tmp_buf, &err);
	if (err != DHARA_E_NONE)
		return 0;
	for (int i=0; i<512; i++)
		if (tmp_buf[i] != 0xff)
			return 0;
	return 1;
}

int dhara_nand_copy(const struct dhara_nand *n, dhara_page_t src, dhara_page_t dst, dhara_error_t *err) {
	dhara_nand_read(&nand, src, 0, 512, tmp_buf, err);
	if (*err != DHARA_E_NONE)
		return -1;

	return dhara_nand_prog(&nand, dst, tmp_buf, err);
}

//--------------------------------------------------------------------+
// blockdev
//--------------------------------------------------------------------+

static uint_fast8_t flash_transfer(void) {
	dhara_error_t err = DHARA_E_NONE;
	if (blockdev[blockdev_id_flash].op->is_read)
		dhara_map_read(&dhara, blockdev[blockdev_id_flash].op->lba, blockdev[blockdev_id_flash].op->addr, &err);
	else
		dhara_map_write(&dhara, blockdev[blockdev_id_flash].op->lba, blockdev[blockdev_id_flash].op->addr, &err);

	// TODO push op in queue
	return (err == DHARA_E_NONE);
}

static int flash_trim(void) {
	dhara_sector_t sector = blockdev[blockdev_id_flash].op->lba;
	if (sector < (nand.num_blocks << nand.log2_ppb))
		dhara_map_trim(&dhara, sector, NULL);

	// TODO push op in queue
	return 0;
}

uint32_t flash_init(void *blk_op) {
	critical_section_init(&flash_critical);

	printf("NOR flash, ");
	dhara_map_init(&dhara, &nand, journal_buf, 10);
	dhara_error_t err = DHARA_E_NONE;
	dhara_map_resume(&dhara, &err);
	uint32_t lba = dhara_map_capacity(&dhara);
	printf("%dkB physical %dkB logical at 0x%p: ", nand.num_blocks * 4, lba / 2, XIP_NOCACHE_NOALLOC_BASE + FLASH_OFFSET);

	blockdev_id_flash = blockdev_add(flash_transfer, NULL, flash_trim, lba, blk_op);

	return lba;
}

/* vim: sw=4 ts=4 et: */

