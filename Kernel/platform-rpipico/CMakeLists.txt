cmake_minimum_required(VERSION 3.12)

# Pull in rt (must be before project)
include(pico_sdk_import.cmake)

project(fuzix C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set(PICO_COPY_TO_RAM 1)

pico_sdk_init()

# TODO need to fix the log in order to be pure macro
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        add_definitions(-DBUILD_DEBUG)
else()
        add_definitions(-DBUILD_RELEASE)
endif()

include_directories(
	.
	..
	../dev
	../include
	../cpu-armm0
)

add_executable(fuzix
	rt_stdio.c
	rt_time.c
	rt_softirq.c
	rt_power.c
	rt_chardev.c
	rt_core1.c
	rt_uart.c
	rt_blockdev.c
	rt_blockdev_flash.c
	rt_blockdev_sd.c
	rt_blockdev_usb.c
	rt_usb.c
	fuzix_elf.c 
	fuzix_tty.c
	fuzix_swapper.c
	fuzix_softirq.c
	fuzix_ticker.c
	fuzix_chardev.c
	fuzix_blockdev.c
	fuzix_misc.c
	devices.c
	main.c
	tricks.S
	../dev/blkdev.c
	../dev/mbr.c
	../dev/devsd_discard.c
	../dev/devsd.c
	../lib/dhara/error.c
	../lib/dhara/journal.c
	../lib/dhara/map.c
	../lib/hexdump.c
	../devio.c
	../devsys.c
	../filesys.c
	../inode.c
	../kdata.c
	../lowlevel-armm0.c
	../malloc.c
	../mm.c
	../process.c
	../start.c
	../swap.c
	../syscall_execelf32.c
	../syscall_fs.c
	../syscall_fs2.c
	../syscall_fs3.c
	../syscall_other.c
	../syscall_proc.c
	../timer.c
	../tty.c
	../usermem.c
	../version.c
)

target_link_libraries(fuzix
	tinyusb_device
	pico_stdlib
	pico_runtime
	pico_time
	pico_unique_id
	pico_multicore
	hardware_regs
	hardware_structs
	hardware_sync
	hardware_watchdog
	hardware_flash
	hardware_spi
	hardware_uart
)

pico_enable_stdio_uart(fuzix 0)
pico_enable_stdio_usb(fuzix 0)
pico_set_float_implementation(fuzix none)
pico_set_double_implementation(fuzix none)
pico_add_extra_outputs(fuzix)
