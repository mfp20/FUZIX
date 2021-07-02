#ifndef _PLATFORM_DOT_H
#define _PLATFORM_DOT_H

#include "platform_sdk.h"
#include "config.h"
#include "devvirt.h"

#include "tusb_config.h"

// signal fuzix is in 'ei'/'irqrestore', ie: core code can be used freely
extern bool fuzix_ready;

// power
typedef enum power_mode_e {
    POWER_DOWN = 0,
    POWER_SAVE,
    POWER_STANDBY,
    POWER_STANDBY_EXTENDED,
    POWER_OFFSET,           // 4
    POWER_LEVEL_USER,
    POWER_LEVEL_SAVE,
    POWER_LEVEL_BALANCED,
    POWER_LEVEL_PERFORMANCE,
    POWER_LEVEL_RAM,        // 9
    POWER_DEFAULT = 255
} power_mode_t;
extern void power_test_start(void);
extern void power_set_speed(power_mode_t i);
extern void power_set_mode(power_mode_t i);

// time
extern uint32_t monotonic32(void);
extern uint64_t monotonic64(void);

// chardev
typedef uint8_t (*byte_rx_t)(void);
typedef void (*byte_tx_t)(uint8_t);
typedef bool (*byte_ready_t)(void);
typedef struct chardev_s {
    byte_rx_t rx;
    byte_tx_t tx;
    byte_ready_t ready;
} chardev_t;
extern chardev_t *chardev;
extern uint8_t chardev_no;
extern uint8_t chardev_console_id;
extern uint8_t chardev_log_id;
extern uint8_t chardev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w);
extern void chardev_set_console(uint8_t tty_id);
extern void chardev_set_log(uint8_t tty_id);

// blockdev
typedef uint8_t (*byte_rx_t)(void);
typedef void (*byte_tx_t)(uint8_t);
typedef bool (*byte_ready_t)(void);
typedef struct blockdev_s {
    byte_rx_t rx;
    byte_tx_t tx;
    byte_ready_t ready;
} blockdev_t;
extern blockdev_t *blockdev;
extern uint8_t blockdev_no;
extern uint8_t blockdev_console_id;
extern uint8_t blockdev_log_id;
extern uint8_t blockdev_add(byte_rx_t r, byte_tx_t t, byte_ready_t w);
extern void blockdev_set_console(uint8_t tty_id);
extern void blockdev_set_log(uint8_t tty_id);

// tty
extern void tty_prepare(void);
extern void tty0_putc(uint8_t c);
extern void tty1_putc(uint8_t c);

// log
extern void log_set_level(uint8_t level);

// core1
typedef void (*core1_main_t)(void);
extern uint8_t core1_read(void);
extern void core1_write(uint8_t b);
extern bool core1_writable(void);
extern bool core1_readable(void);
extern void core1_init(core1_main_t c1main, byte_tx_t rx_cb);

// UARTs
extern void uart0_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t uart0_read(void);
extern void uart0_write(uint8_t b);
extern bool uart0_writable(void);
extern void uart1_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate, byte_tx_t rx_cb);
extern uint8_t uart1_read(void);
extern void uart1_write(uint8_t b);
extern bool uart1_writable(void);
extern void uart_stdio(uint8_t id, bool stdio);

// SPIs
extern void devsd_spi_init(void);

// flash
extern void devflash_init(void);

// USB
extern uint8_t usb_cdc0_read(void);
extern void usb_cdc0_write(uint8_t b);
extern bool usb_cdc0_writable(void);
extern void usb_init(void);
extern void usb_cdc_stdio(uint8_t id, bool stdio);

#endif
