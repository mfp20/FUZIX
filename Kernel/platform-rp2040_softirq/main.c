#include "platform.h"

int main(void)
{
	// stdio early init
	chardev_init();
	// say hello
	log_test_color();

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

	INFO("Pico realtime layer initialized. Fuzix starting...");
	INFO("");

	// disable interrupts and run fuzix
	di();
	fuzix_main();
}

/* vim: sw=4 ts=4 et: */
