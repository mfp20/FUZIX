#ifndef _CONFIG_DOT_H
#define _CONFIG_DOT_H

#include <stdint.h>

// Enable to make ^Z dump the inode table for debug
#undef CONFIG_IDUMP

// Enable to make ^A drop back into the monitor
#undef CONFIG_MONITOR

// Profil syscall support (not yet complete)
#undef CONFIG_PROFIL

// Multiple processes in memory at once
#define CONFIG_MULTI

// 32bit with flat memory
#undef CONFIG_FLAT

// Pure swap
#define CONFIG_BANKS 1

// brk() calls pagemap_realloc() to get more memory.
#define CONFIG_BRK_CALLS_REALLOC

// Inlined irq handling
#define CONFIG_INLINE_IRQ
// Use soft irqs, ie: hard irqs are managed by underlying RT env
#define CONFIG_SOFT_IRQ

// Trim disk blocks when no longer used
#define CONFIG_TRIM

// Enable single tasking
#define CONFIG_SWAP_ONLY
#define CONFIG_SPLIT_UDATA

// Enable SD card code.
#define CONFIG_SD
#define SD_DRIVE_COUNT 0

// Enable dynamic swap.
#define CONFIG_PLATFORM_SWAPCTL

//
#define CONFIG_32BIT
#define CONFIG_USERMEM_DIRECT

// on-board RTC
#define CONFIG_RTC              // enables platform_rtc_secs()
#define CONFIG_RTC_FULL         // enables platform_rtc_read() and platform_rtc_write()
//#define CONFIG_RTC_EXTENDED     // enables platform_rtc_ioctl()
#define CONFIG_RTC_INTERVAL 50  // deciseconds between reading RTC seconds counter

// log config
#ifdef BUILD_DEBUG
#define LOG_LEVEL (7)
#else
#define LOG_LEVEL (6)
#endif
#define LOG_COLOR (1)

// Serial TTY, no VT or font
#undef CONFIG_VT
#undef CONFIG_FONT8X8
#define NUM_DEV_TTY 3
#define BOOT_TTY (512 + 1)  // the device you will use as your 'console' at boot time (513 is the first tty, 514 the second, etc)
#define TTYDEV   (512 + 1)  // device init will use as input/output (usually BOOT_TTY)

// Program layout
#define UDATA_BLKS  3
#define UDATA_SIZE  (UDATA_BLKS << BLKSHIFT)
#define USERMEM (216*1024)
#define PROGSIZE (65536 - UDATA_SIZE)
extern uint8_t progbase[USERMEM];
#define udata (*(struct u_data*)progbase)
#define USERSTACK (4*1024) /* 4kB */
#define CONFIG_CUSTOM_VALADDR
#define PROGBASE ((uaddr_t)&progbase[0])
#define PROGLOAD ((uaddr_t)&progbase[UDATA_SIZE])
#define PROGTOP (PROGLOAD + PROGSIZE)
#define SWAPBASE PROGBASE
#define SWAPTOP (PROGBASE + (uaddr_t)alignup(udata.u_break - PROGBASE, 1<<BLKSHIFT)) /* never swap in/out data above break */
#define SWAP_SIZE   ((PROGSIZE >> BLKSHIFT) + UDATA_BLKS)
#define MAX_SWAPS   (2048*2 / SWAP_SIZE) /* for a 2MB swap partition */

// Ticks per second
#define TICKSPERSEC 1000

// Location of root dev name
// TODO we need a tidier way to do this from the loader
#define CMDLINE	NULL

#define BOOTDEVICE  0x0000      // hda
#define SWAPDEV     (swap_dev)  // dynamic swap
#define NBUFS       20          // Number of block buffers
#define NMOUNTS     4           // Number of mounts at a time
#define MAX_BLKDEV	4
#define FLASH_OFFSET (96*1024)

// USB interfaces
#define USB_DEV_CONSOLE (1) // system console
#define USB_DEV_LOG     (1) // system log
#define USB_DEV_MPLEX   (1) // system binary multiplexer for multiple binary streams (ex: external fs, RPC, ...)
#define USB_DEV_TTY1    (1) // user tty 1
#define USB_DEV_RAW1    (1) // user custom interface 1
#define USB_DEV_TTY2    (1) // user tty 2
#define USB_DEV_RAW2    (1) // user custom interface 2

//
#define swap_map(x) ((uint8_t*)(x))

#define MANGLED 1
#include "mangle.h"

#endif

// vim: sw=4 ts=4 et
