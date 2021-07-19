#include "platform.h"

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

uint8_t sys_cpu = A_ARM;
uint8_t sys_cpu_feat = AF_CORTEX_M0;
uint8_t need_resched;
uaddr_t ramtop = (uaddr_t)PROGTOP;
uint8_t sys_stubs[sizeof(struct exec)];
uint16_t swap_dev = 0xffff;

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

/* Called on return from system calls while on the user stack, immediately
 * before returning into the user program. A signal may longjmp out, so this
 * may not return. */
void deliver_signals(void)
{
        for (;;)
        {
                uint8_t cursig = udata.u_cursig;
                if (!cursig)
                        break;

                typedef int (*sigvec_fn)(int);
                sigvec_fn fn = udata.u_sigvec[cursig];

                /* Semantics for now: signal delivery clears handler. */
                udata.u_sigvec[cursig] = 0;
                udata.u_cursig = 0;

                fn(cursig);
        }
}

void set_cpu_type(void) {}
void map_init(void) {}
void program_vectors(uint16_t* pageptr) {}

// This checks to see if a user-supplied address is legitimate
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
	kprintf("RP2040 platform Copyright (c) 2021 Raspberry Pi Fundation, David Given, Anichang\n");
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

uint_fast8_t platform_suspend() {
	// TODO
	return EOPNOTSUPP;
}

void platform_reboot(void) {
	watchdog_reboot(0,0,0);
}

// /dev/platform
int platform_dev_ioctl(uarg_t request, char *data) {
    if (valaddr((unsigned char *)data, 2))
		switch (request){
//			case IOCTL:
//				return 0;
//			break;
			default:
			break;
		}
	udata.u_error = EINVAL;
	return -1;
}

/* vim: sw=4 ts=4 et: */
