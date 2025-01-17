#include "platform.h"

#include <kdata.h>
#include <vt.h>

struct s_queue ttyinq[NUM_DEV_TTY + 1]; // note: ttyinq[0] is never used
tcflag_t termios_mask[NUM_DEV_TTY + 1];
uint8_t buf[NUM_DEV_TTY][TTYSIZ];

//--------------------------------------------------------------------+
// kputchar
//--------------------------------------------------------------------+

// output for the system log (kprintf etc)
void kputchar(uint_fast8_t c)
{
	if (c == '\n')
		chardev[tty_cd[0]].tx('\r');
	chardev[tty_cd[0]].tx(c);
}

//--------------------------------------------------------------------+
// fuzix API
//--------------------------------------------------------------------+

// called on tty_open() and ioctl
void tty_setup(uint_fast8_t minor, uint_fast8_t flags)
{
    //kprintf("tty%d setup\n", minor);
}

// return 1 if connected
int tty_carrier(uint_fast8_t minor)
{
    kprintf("tty%d carrier\n", minor);
    // tty0 is always connected so that
    // it is easier to connect 2-pins uart
    if (minor == 1)
        return 1;

    // TODO rts/cts? dtr/dsr? cd? break?
    return 1;
}

// called at the end of tty_read()
void tty_data_consumed(uint_fast8_t minor)
{
    //kprintf("tty%d data_consumed\n", minor);
}

ttyready_t tty_writeready(uint_fast8_t minor)
{
    //kprintf("tty%d writeready\n", minor);
    if (chardev[tty_cd[minor]].ready)
        return chardev[tty_cd[minor]].ready() ? TTY_READY_NOW : TTY_READY_SOON;
    return TTY_READY_LATER;
}

void tty_putc(uint_fast8_t minor, uint_fast8_t c)
{
    if (chardev[tty_cd[minor]].tx)
    {
        if (c == '\n')
            chardev[tty_cd[minor]].tx('\r');
        chardev[tty_cd[minor]].tx(c);
    }
}

// called when tty isn't ready
void tty_sleeping(uint_fast8_t minor)
{
    kprintf("tty_sleeping minor: %d\n", minor);
}

//--------------------------------------------------------------------+
// rt API
//--------------------------------------------------------------------+

void fuzix_ttys_prepare(void)
{
    // ttyinq[0] is never used
    ttyinq[0] = (struct s_queue){0, 0, 0, 0, 0, 0};
    termios_mask[0] = 0;
    for (int i = 1; i < NUM_DEV_TTY + 1; i++)
    {
        ttyinq[i] = (struct s_queue){buf[i], buf[i], buf[i], TTYSIZ, 0, TTYSIZ / 2};
        termios_mask[i] = _CSYS;
    }
}

void fuzix_tty1_write(uint8_t b) {
	tty_inproc(minor((512 + 1)), b);
}

void fuzix_tty2_write(uint8_t b) {
	tty_putc(minor((512 + 2)), b);
}

void fuzix_tty3_write(uint8_t b) {
	tty_putc(minor((512 + 3)), b);
}

/* vim: sw=4 ts=4 et: */
