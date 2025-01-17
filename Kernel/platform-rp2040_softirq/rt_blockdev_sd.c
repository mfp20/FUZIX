
#include "config.h"
#include "rt_log.h"
#include "rt_softirq.h"
#include "rt_blockdev.h"
#include "rt_blockdev_sd.h"

#include <stdio.h>
#include <string.h>

uint8_t blockdev_id_sd = 0;

static critical_section_t sd_critical;


//--------------------------------------------------------------------+
// low level
//--------------------------------------------------------------------+

/*
void sd_spi_clock(bool go_fast) {
    spi_set_baudrate(spi1, go_fast ? 4000000 : 250000);
}

void sd_spi_raise_cs(void) {
    gpio_put(1<<13, true);
}

void sd_spi_lower_cs(void) {
    gpio_put(1<<13, false);
}

void sd_spi_transmit_byte(uint_fast8_t b) {
    spi_write_blocking(spi1, (uint8_t*) &b, 1);
}

uint_fast8_t sd_spi_receive_byte(void) {
    uint8_t b;
    spi_read_blocking(spi1, 0xff, (uint8_t*) &b, 1);
    return b;
}

bool sd_spi_receive_sector(void) {
    spi_read_blocking(spi1, 0xff, (uint8_t*) blk_op.addr, 512);
	return 0;
}

bool sd_spi_transmit_sector(void) {
    spi_write_blocking(spi1,  (uint8_t*) blk_op.addr, 512);
	return 0;
}

void devsd_spi_init(void) {
    gpio_init_mask(0xf << 12);
    gpio_set_function(12, GPIO_FUNC_SPI);
    gpio_set_function(13, GPIO_FUNC_SIO);
    gpio_set_function(14, GPIO_FUNC_SPI);
    gpio_set_function(15, GPIO_FUNC_SPI);
    gpio_set_dir(13, true);

    spi_init(spi1, 250000);
    spi_set_format(spi1, 8, 0, 0, SPI_MSB_FIRST);
}
*/

//--------------------------------------------------------------------+
// blockdev
//--------------------------------------------------------------------+

static uint_fast8_t sd_transfer(void) {

	return 1;  // success
}

static int sd_trim(void) {

	return 0;
}

uint32_t sd_init(void *blk_op) {
	critical_section_init(&sd_critical);

    if (SD_DRIVE_COUNT>0)
    	WARN("SD card, NOT IMPLEMENTED");

	// TODO
    //devsd_spi_init();
	//devsd_init();

	//printf("%dkB physical %dkB logical at 0x%p: ", nand.num_blocks * 4, lba / 2, XIP_NOCACHE_NOALLOC_BASE + FLASH_OFFSET);

	//blockdev_id_sd = blockdev_add(sd_transfer, NULL, sd_trim, lba, blk_op);

	//return lba;
    return 0;
}

/* vim: sw=4 ts=4 et: */

