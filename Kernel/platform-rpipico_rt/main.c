#include "platform.h"

#include <kernel.h>
#include <kdata.h>
#include <kernel-armm0.def>
#include <printf.h>

struct svc_frame
{
	uint32_t r12;
	uint32_t pc;
	uint32_t lr;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
};

struct exception_frame
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};

struct extended_exception_frame
{
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t cause;
	uint32_t sp;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};

void fatal_exception_handler(struct extended_exception_frame *eh)
{
	kprintf("FLAGRANT SYSTEM ERROR! EXCEPTION %d\n", eh->cause);
	kprintf(" r0=%p r1=%p  r2=%p  r3=%p\n", eh->r0, eh->r1, eh->r2, eh->r3);
	kprintf(" r4=%p r5=%p  r6=%p  r7=%p\n", eh->r4, eh->r5, eh->r6, eh->r7);
	kprintf(" r8=%p r9=%p r10=%p r11=%p\n", eh->r8, eh->r9, eh->r10, eh->r11);
	kprintf("r12=%p sp=%p  lr=%p  pc=%p\n", eh->r12, eh->sp, eh->lr, eh->pc);
	kprintf("PROGBASE=%p PROGLOAD=%p PROGTOP=%p\n", PROGBASE, PROGLOAD, PROGTOP);
	kprintf("UDATA=%p KSTACK=%p-%p\n", &udata, &udata + 1, ((uint32_t)&udata) + UDATA_SIZE);
	kprintf("user mode relative: lr=%p pc=%p isp=%p brk=%p\n",
			eh->lr - PROGLOAD, eh->pc - PROGLOAD, udata.u_isp, udata.u_break);
	panic("fatal exception");
}

void syscall_handler(struct svc_frame *eh)
{
	udata.u_callno = *(uint8_t *)(eh->pc - 2);
	udata.u_argn = eh->r0;
	udata.u_argn1 = eh->r1;
	udata.u_argn2 = eh->r2;
	udata.u_argn3 = eh->r3;
	udata.u_insys = 1;

	unix_syscall();

	udata.u_insys = 1;
	eh->r0 = udata.u_retval;
	eh->r1 = udata.u_error;
}

int main(void)
{
	// sanity check
	if ((U_DATA__U_SP_OFFSET != offsetof(struct u_data, u_sp)) ||
		(U_DATA__U_PTAB_OFFSET != offsetof(struct u_data, u_ptab)) ||
		(P_TAB__P_PID_OFFSET != offsetof(struct p_tab, p_pid)) ||
		(P_TAB__P_STATUS_OFFSET != offsetof(struct p_tab, p_status)) ||
		(UDATA_SIZE_ASM != UDATA_SIZE))
	{
		kprintf("U_DATA__U_SP = %d\n", offsetof(struct u_data, u_sp));
		kprintf("U_DATA__U_PTAB = %d\n", offsetof(struct u_data, u_ptab));
		kprintf("P_TAB__P_PID_OFFSET = %d\n", offsetof(struct p_tab, p_pid));
		kprintf("P_TAB__P_STATUS_OFFSET = %d\n", offsetof(struct p_tab, p_status));
		panic("bad offsets");
	}

	// TODO calculate USERMEM at boot time from the end address of the OS and top of usable memory
	ramsize = (SRAM_END - SRAM_BASE) / 1024;
	procmem = USERMEM / 1024;

	// stdio early init
	chardev_init();

	// init rtc, alarm pools, ...
	time_init();

	// setup softirq
	softirq_init();

	// power
	//power_set_mode(POWER_DEFAULT);

	// usb
	usb_init();

    // tweak real irqs priorities
	// NOTE: all IRQs are set to priority 128 by default at boot
    // RTC: highest irq priority, for jitter evaluation/correction every second
    irq_set_priority(RTC_IRQ, 4);
	// TIMER0: high priority timer (use with care)
	irq_set_priority(TIMER_IRQ_0, 8);
	// FLASH and USB low level
	irq_set_priority(XIP_IRQ, 16);
	irq_set_priority(USBCTRL_IRQ, 16);
	// TIMER1: system timer (fuzix ticker)
	//irq_set_priority(TIMER_IRQ_1, 32); // bug, system hangs if enabled
	// TIMER2: low priority timer
	irq_set_priority(TIMER_IRQ_2, 64);
	// TIMER3: softirq and usb maintenance (default pool hooks on this timer by default)
	irq_set_priority(TIMER_IRQ_3, 128);

	// say hello
	log_test_color();

	INFO("");
	INFO("Pico realtime layer initialized");
	INFO("");

	// disable interrupts and run fuzix
	di();
	fuzix_main();
}

/* vim: sw=4 ts=4 et: */
