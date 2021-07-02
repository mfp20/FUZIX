#include "platform.h"

#include <kernel.h>
#include <kdata.h>
#include <printf.h>
#include <vt.h>
#include <tty.h>

// virtual tty definition
typedef struct tty_def_s {
    uint8_t buf[TTYSIZ];
    byte_rx_t rx;
    byte_tx_t tx;
    byte_ready_t writable;
} tty_def_t;

// all virtual ttys
static tty_def_t tty[NUM_DEV_TTY];

// console virtual tty
static uint8_t console = 0;

// fuzix ttys, note: ttyinq[0] is never used
struct s_queue ttyinq[NUM_DEV_TTY+1];
tcflag_t termios_mask[NUM_DEV_TTY+1];

// output for the system console (kprintf etc)
void kputchar(uint_fast8_t c) {
    if (tty[console].tx) {
        if (c=='\n')
            tty[console].tx('\r');
        tty[console].tx(c);
    }
}

ttyready_t tty_writeready(uint_fast8_t minor) {
    if (tty[minor-1].writable)
        return tty[minor-1].writable() ? TTY_READY_NOW : TTY_READY_SOON;
    return TTY_READY_LATER;
}

// called when tty isn't ready
void tty_sleeping(uint_fast8_t minor) {}

void tty_putc(uint_fast8_t minor, uint_fast8_t c) {
    if (tty[minor-1].tx) {
//        if (c=='\n')
//            tty[minor].tx('\r');
        tty[minor-1].tx(c);
    } 
}

// called on tty_open() and ioctl
void tty_setup(uint_fast8_t minor, uint_fast8_t flags) {}

// return 1 if connected
int tty_carrier(uint_fast8_t minor) {
    // tty0 is always connected so that
    // it is easier to connect 2 pins uart
    if (minor==1)
        return 1;
    
    // TODO rts/cts? dtr/dsr? cd? break?
    return 1;
}

// called at the end of tty_read()
void tty_data_consumed(uint_fast8_t minor) {}


//--------------------------------------------------------------------+
// 
//--------------------------------------------------------------------+

void devtty_init(void) {
    // ttyinq[0] is never used
    ttyinq[0] = (struct s_queue){ 0, 0, 0, 0, 0, 0 };
    termios_mask[0] = 0;
    for (int i=1;i<NUM_DEV_TTY+1;i++) {
        tty[i-1].tx = NULL;
        tty[i-1].rx = NULL;
        tty[i-1].writable = NULL;
        ttyinq[i] = (struct s_queue){ tty[i-1].buf, tty[i-1].buf, tty[i-1].buf, TTYSIZ, 0, TTYSIZ/2 };
        termios_mask[i] = _CSYS;
    }
}

void devtty_bind(uint8_t n, byte_rx_t r, byte_tx_t t, byte_ready_t w) {
    tty[n].rx = r;
    tty[n].tx = t;
    tty[n].writable = w;
}

void devtty_set_console(uint8_t con) {
    console = con;
}

void tty0_putc(uint8_t c) {
	tty_inproc(minor((512 + 1)), c);
}

void tty1_putc(uint8_t c) {
	tty_inproc(minor((512 + 2)), c);
}

/* vim: sw=4 ts=4 et: */
