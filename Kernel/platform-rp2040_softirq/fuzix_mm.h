#ifndef _FUZIX_MM_PAGEMAPPER_DOT_H
#define _FUZIX_MM_PAGEMAPPER_DOT_H

#include "fuzix_platform.h"

/* 
 * A special memory manager implementation (pagemapper and swapper) for the Raspberry Pi Pico.
 * Available memory is divided into 4kB chunks; processes can occupy any number of these
 * (up to 64kB). When a process is switched in, memory is rearranged by swapping chunks
 * until the running process is at the bottom of memory in the right order.
 *
 * p->p_page is 0 if swapped out, 1 is swapped in.
 */

typedef struct pagemap_entry_s {
    uint8_t slot;
    uint8_t block;
} pagemap_entry_t;

extern uint16_t ramsize;
extern uint16_t procmem;
extern uint8_t progbase[USERMEM];

void contextswitch(ptptr p);
void clonecurrentprocess(ptptr p);

int swapout(ptptr p);
void swapin(ptptr p, uint16_t map);

#endif