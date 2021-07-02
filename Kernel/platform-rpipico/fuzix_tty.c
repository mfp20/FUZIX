#include "platform.h"

#include <kernel.h>
#include <kdata.h>
#include <printf.h>
#include <vt.h>
#include <tty.h>

extern chardev_t *chardev;
extern uint8_t chardev_no;

// fuzix ttys, note: ttyinq[0] is never used
uint8_t buf[NUM_DEV_TTY][TTYSIZ];
struct s_queue ttyinq[NUM_DEV_TTY+1];
tcflag_t termios_mask[NUM_DEV_TTY+1];

// setup ttys
void tty_prepare(void) {
    // ttyinq[0] is never used
    ttyinq[0] = (struct s_queue){ 0, 0, 0, 0, 0, 0 };
    termios_mask[0] = 0;
    for (int i=1;i<NUM_DEV_TTY+1;i++) {
        ttyinq[i] = (struct s_queue){ buf[i], buf[i], buf[i], TTYSIZ, 0, TTYSIZ/2 };
        termios_mask[i] = _CSYS;
    }
}

// output for the system console (kprintf etc)
void kputchar(uint_fast8_t c) {
    if (chardev[chardev_console_id].tx) {
        if (c=='\n')
            chardev[chardev_console_id].tx('\r');
        chardev[chardev_console_id].tx(c);
    }
}

// called on tty_open() and ioctl
void tty_setup(uint_fast8_t minor, uint_fast8_t flags) {}

// return 1 if connected
int tty_carrier(uint_fast8_t minor) {
    // tty0 is always connected so that
    // it is easier to connect 2-pins uart
    if (minor==1)
        return 1;
    
    // TODO rts/cts? dtr/dsr? cd? break?
    return 1;
}

// called at the end of tty_read()
void tty_data_consumed(uint_fast8_t minor) {}

ttyready_t tty_writeready(uint_fast8_t minor) {
    if (chardev[minor-1].ready)
        return chardev[minor-1].ready() ? TTY_READY_NOW : TTY_READY_SOON;
    return TTY_READY_LATER;
}

void tty_putc(uint_fast8_t minor, uint_fast8_t c) {
    if (chardev[minor-1].tx) {
        if (c=='\n')
            chardev[minor-1].tx('\r');
        chardev[minor-1].tx(c);
    } 
}

// called when tty isn't ready
void tty_sleeping(uint_fast8_t minor) {}

//
void tty0_putc(uint8_t c) {
	tty_inproc(minor((512 + 1)), c);
}
void tty1_putc(uint8_t c) {
	tty_inproc(minor((512 + 2)), c);
}

/* vim: sw=4 ts=4 et: */
