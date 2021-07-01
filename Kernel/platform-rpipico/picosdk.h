#define MANGLED 0
#include "mangle.h"

//#include <RP2040.h> // NVIC_SystemReset() to reboot cpu

#include <pico.h>

#include <pico/stdlib.h>
#include <pico/types.h>
#include <pico/unique_id.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>

#include <hardware/regs/usb.h>

#include <hardware/structs/vreg_and_chip_reset.h>
#include <hardware/structs/timer.h>
#include <hardware/structs/usb.h>

#include <hardware/vreg.h>
#include <hardware/sync.h>
#include <hardware/watchdog.h>
#include <hardware/irq.h>
#include <hardware/resets.h>
#include <hardware/flash.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <hardware/spi.h>

typedef queue_t pico_queue_t;

#define MANGLED 1
#include "mangle.h"

#include <stddef.h>
#include <stdbool.h>

typedef void (*core1_main_t)(void);
