#include "platform.h"

#include "fuzix_rt.h"

#include <kernel.h>
#include <kdata.h>
#include <printf.h>
#include <exec.h>

uint8_t sys_cpu = A_ARM;
uint8_t sys_cpu_feat = AF_CORTEX_M0;
uint8_t need_resched;
uaddr_t ramtop = (uaddr_t) PROGTOP;
uint8_t sys_stubs[sizeof(struct exec)];
uint16_t swap_dev = 0xffff;

void set_cpu_type(void) {}
void map_init(void) {}
void program_vectors(uint16_t* pageptr) {}

uaddr_t pagemap_base(void) {
    return PROGBASE;
}

usize_t valaddr(const uint8_t *base, usize_t size) {
	if (base + size < base)
		size = MAXUSIZE - (usize_t)base + 1;
	if (!base || base < (const uint8_t *)PROGBASE)
		size = 0;
	else if (base + size > (const uint8_t *)(size_t)udata.u_ptab->p_top)
		size = (uint8_t *)(size_t)udata.u_ptab->p_top - base;
	if (size == 0)
		udata.u_error = EFAULT;
	return size;
}

// output for the system log (kprintf etc)
void kputchar(uint_fast8_t c)
{
	if (c == '\n')
		stdio_putchar('\r');
	stdio_putchar(c);
}

void platform_copyright(void) {
	kprintf("RP2040 platform Copyright (c) 2021 RPi Fundation, David Given, Anichang\n");
}

uint_fast8_t platform_param(char* p) {
	return 0;
}

uint_fast8_t platform_canswapon(uint16_t devno) {
    // Only allow swapping to hd devices.
    return (devno >> 8) == 0;
}

void platform_idle(void) {
	// flush softirqs
	fuzix_softirq();
	// go into power save
	//power_set_mode(POWER_LEVEL_SAVE);
	// wait for interrupt
	__wfi();
	// resume normal power level
	//power_set_mode(POWER_LEVEL_USER);
} 

void platform_discard(void) {}

void platform_monitor(void) {}

void platform_reboot(void) {
	watchdog_reboot(0,0,0);
}

/* vim: sw=4 ts=4 et: */
