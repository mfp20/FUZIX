cmake_minimum_required(VERSION 3.12)

# Pull in rt (must be before project)
include(pico_sdk_import.cmake)

project(fuzix C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

set(CMAKE_BUILD_TYPE Debug)
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
# Pico realtime layer code
	rt_time.c
	rt_stdio.c
	rt_softirq.c
	rt_power.c
	rt_chardev.c
	rt_core1.c
	rt_uart.c
	rt_blockdev.c
	rt_blockdev_flash.c
	rt_blockdev_sd.c
	rt_blockdev_usb.c
	rt_usb_mplex.c
	rt_usb.c
# Pico port code
	fuzix_elf.c 
	fuzix_swapper.c
	fuzix_softirq.c
	fuzix_platform.c
	fuzix_devblock.c
	fuzix_devtty.c
	fuzix_devmem.c
	fuzix_devrtc.c
#	fuzix_devgpio.c
#	fuzix_devi2c.c
#	fuzix_devaudio.c
#	fuzix_devinput.c
	devices.c
	tricks.S
# Fuzix code
	../version.c
	../lowlevel-armm0.c
	../malloc.c
	../mm.c
	../process.c
	../start.c
	../filesys.c
	../inode.c
	../kdata.c
	../swap.c
	../usermem.c
	../syscall_execelf32.c
	../syscall_fs.c
	../syscall_fs2.c
	../syscall_fs3.c
	../syscall_other.c
	../syscall_proc.c
	../timer.c
	../tty.c
	../audio.c
	../devio.c
	../devsys.c
#	../devinput.c
	../dev/blkdev.c
	../dev/mbr.c
	../dev/devsd_discard.c
	../dev/devsd.c
#	../dev/i2c.c
	../lib/dhara/error.c
	../lib/dhara/journal.c
	../lib/dhara/map.c
	../lib/hexdump.c
# main()
	main.c
)

target_link_libraries(fuzix
	tinyusb_device
	pico_stdlib
	pico_runtime
	pico_util
	pico_time
	pico_unique_id
	pico_multicore
	hardware_regs
	hardware_structs
	hardware_sync
	hardware_watchdog
	hardware_rtc
	hardware_flash
	hardware_spi
	hardware_uart
)

pico_enable_stdio_uart(fuzix 0)
pico_enable_stdio_usb(fuzix 0)
pico_set_float_implementation(fuzix none)
pico_set_double_implementation(fuzix none)
pico_add_extra_outputs(fuzix)
