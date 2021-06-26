
#include "platform.h"

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

spin_lock_t *fuzix_core_lock;

//uint32_t di(void) {
//	spin_lock_unsafe_blocking(fuzix_core_lock);
//	return 0;
//}

//void ei(void) {}

//void irqrestore(uint32_t ps) {
//	spin_unlock_unsafe(fuzix_core_lock);
//}

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

void platform_discard(void) {}

void platform_monitor(void) {}

void platform_reboot(void) {
	//NVIC_SystemReset();
}

/* vim: sw=4 ts=4 et: */
