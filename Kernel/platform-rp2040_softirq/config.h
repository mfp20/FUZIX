#ifndef _CONFIG_DOT_H
#define _CONFIG_DOT_H

//--------------------------------------------------------------------+
// Platform specific: 6502-based
//--------------------------------------------------------------------+
//
//#define CONFIG_RAMWORKS

//--------------------------------------------------------------------+
// Platform specific: 8086-based
//--------------------------------------------------------------------+
//
//#define CONFIG_EMM_BIOS
//
//#define CONFIG_KBD_XT
//
//#define CONFIG_KEYMAP_DE
//
//#define CONFIG_KEYMAP_SE
//
//#define CONFIG_IBMPC_EMM

//--------------------------------------------------------------------+
// Platform specific: PDP11
//--------------------------------------------------------------------+
//
//#define CONFIG_PDP11_04

//--------------------------------------------------------------------+
// Platform specific: 6809-based
//--------------------------------------------------------------------+
//
//#define CONFIG_COCOIDE
//
//#define CONFIG_COCO_KBD
//
//#define CONFIG_COCOSDC
//
//#define CONFIG_COCOSDFPGA
//
//#define CONFIG_COCOSDNANO

//--------------------------------------------------------------------+
// Platform specific: RP2040 (Raspberry Pi Pico)
//--------------------------------------------------------------------+
//
#include <stdint.h>
#define USERMEM (216*1024)
extern uint8_t progbase[USERMEM];
#define udata (*(struct u_data*)progbase)
//
#define USERSTACK (4*1024) // 4kB
//
#define UDATA_BLKS  3
#define UDATA_SIZE  (UDATA_BLKS << BLKSHIFT)
#define PROGSIZE (65536 - UDATA_SIZE)
//
#define FLASH_OFFSET (96*1024)
// USB
#define USB_DEV_TTY1        (1) // system console
#define USB_DEV_TTY2        (1) // system log
#define USB_DEV_MPLEX_SYS   (1) // system binary multiplexer for multiple binary streams (ex: external fs, RPC, ...)
#define USB_DEV_TTY3        (1) // user tty 1
#define USB_DEV_MPLEX_USR   (1) // user custom interface 1
#define USB_DEV_CDC         (1) // user tty 2
#define USB_DEV_VENDOR      (1) // user custom interface 2
#define USB_MPLEX_TIMEOUT   (500) // on boot connection timeout in milliseconds, 0 to disable USB on boot
// log config
#ifdef BUILD_DEBUG
#define LOG_LEVEL (7)
#else
#define LOG_LEVEL (6)
#endif
#define LOG_COLOR (1)

//--------------------------------------------------------------------+
// Platform specific: Z80-based
//--------------------------------------------------------------------+
// floppy disk interface
//#define CONFIG_BETADISK
// Enables FDC765 floppy disk controller
//#define CONFIG_FDC765_MULTISECTOR
// tty, the video is directly mapped
//#define CONFIG_GFXBASE
//
//#define CONFIG_MULTI_IDE
// P112 port: floppy support
//#define CONFIG_P112_FLOPPY
//
//#define CONFIG_PPIDE
//
//#define CONFIG_PPP
//
//#define CONFIG_PROPIO2
//
//#define CONFIG_RC2014_CF
//
//#define CONFIG_RC2014_FLOPPY
//
//#define CONFIG_RC2014_NET
//
//#define CONFIG_RC2014_PPIDE
//
//#define CONFIG_RC2014_SD
//
//#define CONFIG_RC2014_VFD
// tty, timex modes are available
//#define CONFIG_TIMEX
//
//#define CONFIG_VFD_TERM

//--------------------------------------------------------------------+
// general options
//--------------------------------------------------------------------+
// clock rate
#define TICKSPERSEC 1000
// low memory address for applications (should be 0x100 to run standard binaries)
#define PROGBASE ((uaddr_t)&progbase[0])
// In most cases PROGBASE is 0 and PROGLOAD is 0x100. In all cases, PROGBASE<=PROGLOAD.
#define PROGLOAD ((uaddr_t)&progbase[UDATA_SIZE])
// first byte above main memory, usually the start of the udata area and common memory
#define PROGTOP (PROGLOAD + PROGSIZE)
// pointer to a null-terminated command line passed from the loader/firmware. Set to NULL if none
#define CMDLINE NULL
// Enables kernel32.h on 32bits ports
#define CONFIG_32BIT
// Enables process accounting
//#define CONFIG_ACCT
// Inlined irq handling
#define CONFIG_INLINE_IRQ
//
//#define CONFIG_LEVEL_0
//
//#define CONFIG_LEVEL_2
// no timer interrupt available
//#define CONFIG_NO_CLOCK
// Enables suspend to RAM, platform_suspend()
#define CONFIG_PLATFORM_SUSPEND
// Profil syscall support (not yet complete)
#undef CONFIG_PROFIL
// Use soft irqs
#define CONFIG_SOFT_IRQ
//
//#define CONFIG_UDATA_TEXTTOP

//--------------------------------------------------------------------+
// memory banks
//--------------------------------------------------------------------+
// Selects support for four flexible 16K bank registers
//#define CONFIG_BANK16
//
//#define CONFIG_BANK16FC
//
//#define CONFIG_BANK16_LOW
// Selects support for two flexible 32K bank registers
//#define CONFIG_BANK32
//
//#define CONFIG_BANK_65C816
//
//#define CONFIG_BANK8
//
//#define CONFIG_BANK_8086
//
//#define CONFIG_BANKED
// Selects support for lower memory bank selector / fixed common at the top
//#define CONFIG_BANK_FIXED
// Selects a system where memory is managed linearly with base/limit pairs or similar and a common
//#define CONFIG_BANK_LINEAR
//
#define CONFIG_BANKS 1
// 32K common, ldir copying to use for 48K apps
//#define CONFIG_COMMON_COPY
//
//#define CONFIG_HAS_LOW_PAGE
//
//#define CONFIG_LOW_PAGE
//
//#define CONFIG_SUPERVISOR_SPACE
//
//#define CONFIG_VIDMAP8

//--------------------------------------------------------------------+
// block devices
//--------------------------------------------------------------------+
// total number of block devices
#define MAX_BLKDEV	5
//
#define BOOTDEVICE  0x0000 // hda
// swap device, a variable for dynamic or a device major/minor
#define SWAPDEV     (swap_dev)

// number of buffers for disk cache (each a bit of 512 bytes)
#define NBUFS       20
// number of things that can be mounted at a time
#define NMOUNTS     4
//
//#define CONFIG_FLOPPY
//#define CONFIG_FLOPPY_NOHD
//
//#define CONFIG_IDE
//#define CONFIG_IDE_BSWAP
//#define IDE_DRIVE_COUNT 0
//
//#define CONFIG_SCSI
// Enable SD card code
#define CONFIG_SD
#define SD_DRIVE_COUNT 0 // Number of SD cards
//
//#define CONFIG_RAMDISK
// On systems with 32bit blkno_t we support larger block sizes (not supported, yet)
//#define CONFIG_BIG_FS
//
//#define CONFIG_BLKBUF_EXTERNAL
//
//#define CONFIG_BLKBUF_HELPERS
//
//#define CONFIG_BLOCK_SLEEP
//
//#define CONFIG_GPT
// Enable to make ^Z dump the inode table for debug
#undef CONFIG_IDUMP
// Tells dev/mbr.c where to find the MBR
//#define CONFIG_MBR_OFFSET
// Trim disk blocks when no longer used
#define CONFIG_TRIM
//
//#define CONFIG_LARGE_IO_DIRECT

//--------------------------------------------------------------------+
// ram and swap
//--------------------------------------------------------------------+
// lowest address that will be swapped
#define SWAPBASE PROGBASE
// highest address that will be swapped
#define SWAPTOP (PROGBASE + (uaddr_t)alignup(udata.u_break - PROGBASE, 1<<BLKSHIFT)) // never swap in/out data above break
// special use (set to 0)
//#define UDATA_BLOCKS 0
//
//#define UDATA_SWAPSIZE
// number of disk blocks needed to hold all the swap for one process
#define SWAP_SIZE ((PROGSIZE >> BLKSHIFT) + UDATA_BLKS)
// largest number of SWAP_SIZE chunks that will be used
#define MAX_SWAPS (2048*2 / SWAP_SIZE) // for a 2MB swap partition
// brk() calls pagemap_realloc() to get more memory
#define CONFIG_BRK_CALLS_REALLOC
// Enables buddy allocator
//#define CONFIG_BUDDY_MALLOC
// Runtime stacks arguments backwards
//#define CONFIG_CALL_R2L
//
#define CONFIG_CUSTOM_VALADDR
//
//#define CONFIG_DYNAMIC_BUFPOOL
// Memory allocation on systems with a single flat address space and no bank or limit registers
#undef CONFIG_FLAT
//
//#define CONFIG_INDIRECT_QUEUES
// swap: multiple processes in memory at once
#define CONFIG_MULTI
// pure swap config: always give the parent a chance to get to waitpid()
//#define CONFIG_PARENT_FIRST
// Enable dynamic swap
#define CONFIG_PLATFORM_SWAPCTL
// No fast swap device, limited memory
//#define CONFIG_SINGLETASK
//
#define CONFIG_SPLIT_UDATA
//
//#define CONFIG_STACKSIZE
//
//#define CONFIG_SWAP
// Selects NO user banking
#define CONFIG_SWAP_ONLY
// Tells the kernel to use the standard C helpers for user memory copying
//#define CONFIG_USERMEM_C
//
#define CONFIG_USERMEM_DIRECT
//
//#define CONFIG_VMMU
// Search for the first swap partition on MBR partitioned disks
//#define CONFIG_DYNAMIC_SWAP
//
#define swap_map(x) ((uint8_t*)(x))

//--------------------------------------------------------------------+
// tty, gfx
//--------------------------------------------------------------------+
// the device you will use as your 'console' at boot time. 513 is the first tty, 514 the second etc
#define BOOT_TTY (512 + 1)
// device init will use as input/output (usually BOOT_TTY)
#define TTYDEV BOOT_TTY
// number of tty devices (video consoles/serial)
#define NUM_DEV_TTY 3
//
#undef CONFIG_VT
//
//#define CONFIG_VT_MULTI
//
//#define CONFIG_VT_SIMPLE
// Includes fonts
//#define CONFIG_FONT_4X6
//#define CONFIG_FONT_4x8
//#define CONFIG_FONT6X8
#undef CONFIG_FONT8X8
//#define CONFIG_FONT_8X8_EXP2
//#define CONFIG_FONT8X8SMALL
// Enable to make ^A drop back into the monitor
#undef CONFIG_MONITOR
//
//#define CONFIG_PTY_DEV
//
//#define CONFIG_UNIKEY

//--------------------------------------------------------------------+
// RTC
//--------------------------------------------------------------------+
// Enables platform_rtc_secs()
#define CONFIG_RTC
// Enables platform_rtc_read() and platform_rtc_write()
//#define CONFIG_RTC_EXTENDED
// Enables platform_rtc_ioctl()
//#define CONFIG_RTC_FULL
// Number of deciseconds between platform_rtc_secs() calls
#define CONFIG_RTC_INTERVAL 25
//
//#define CONFIG_RTC_REG_NVRAM
//
//#define CONFIG_RTC_RP5C01_NVRAM

//--------------------------------------------------------------------+
// Network
//--------------------------------------------------------------------+
//
//#define CONFIG_NC200
//
//#define CONFIG_NET
//
//#define CONFIG_NET_NATIVE
//
//#define CONFIG_NET_WIZNET
//
//#define CONFIG_NET_WIZNET5300
//
//#define CONFIG_WIZNET_INDIRECT

//--------------------------------------------------------------------+
// misc devices
//--------------------------------------------------------------------+
// Enables audio core support
//#define CONFIG_AUDIO
//
//#define CONFIG_DEV_GPIO
//
//#define CONFIG_DEV_I2C
//
//#define CONFIG_DEV_MEM
//
//#define CONFIG_DEV_PLATFORM
//
//#define CONFIG_DEV_PTY
//
//#define CONFIG_INPUT
//
//#define CONFIG_INPUT_GRABMAX
// Enables generic printer interface
//#define CONFIG_LP_GENERIC

//--------------------------------------------------------------------+
// Delete? Don't exist in code (config.h only). 
//--------------------------------------------------------------------+
//#define CONFIG_CPM_EMU
//#define CONFIG_LEGACY_EXEC
//#define CONFIG_PAGE_SIZE    16

#define MANGLED 1
#include "mangle.h"

#endif
