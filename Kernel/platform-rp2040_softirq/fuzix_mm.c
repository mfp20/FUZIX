#include "platform.h"
#include "fuzix_mm.h"

#include <stdlib.h>

#undef DEBUG

#define BLOCKSIZE 4096
#define NUM_ALLOCATION_BLOCKS (USERMEM / BLOCKSIZE)

/*
The total amount of memory available in the system, including memory used for
the kernel and other resources. This should reflect the way the platform
normal reports total memory. It is used solely to provide feedback to the user
on their system configuration. It should thus match the values reported by
firmware or other convention.
*/
uint16_t ramsize;

/*
Set by the platform the number of Kbytes of memory available for process
allocation. This is used by tools such as 'free' to help report memory
utilization.
*/
uint16_t procmem;

//
uint8_t progbase[USERMEM];

// allocation table
pagemap_entry_t allocation_map[NUM_ALLOCATION_BLOCKS];

//--------------------------------------------------------------------+
// helper methods
//--------------------------------------------------------------------+

//
static uint8_t get_slot(ptptr p)
{
	uint8_t slot = p - ptab;
    if (slot >= PTABSIZE)
        panic("bad ptab");
}

//
static uaddr_t get_proc_size(ptptr p)
{
    if (!p)
        return 0;
    /* init is initially created with a p_top of 0, but it actually needs 512 bytes. */
    if (!p->p_top)
        p->p_top = PROGLOAD + 512;
    return p->p_top - PROGBASE;
}

//
static int get_proc_size_blocks(ptptr p)
{
    return (uaddr_t)alignup(get_proc_size(p), BLOCKSIZE) / BLOCKSIZE;
}

//
static pagemap_entry_t* find_block(uint8_t slot, uint8_t block)
{
    for (int i=0; i<NUM_ALLOCATION_BLOCKS; i++)
    {
        pagemap_entry_t* b = &allocation_map[i];
        if ((b->slot == slot) && (b->block == block))
            return b;
    }

    return NULL;
}

//
static pagemap_entry_t* find_free_block(ptptr p)
{
    for (;;)
    {
        pagemap_entry_t* b = find_block(0xff, 0xff); /* find a free block */
        if (b)
            return b;

        #ifdef DEBUG
            kprintf("alloc failed, finding a process to swap out");
        #endif
        if (!swapneeded(p, true))
        {
            kprintf("WARN: out of memory\n");
            return NULL;
        }
    }
}

#ifdef DEBUG
//
static void debug_blocks(void)
{
    kprintf("current process size %p bytes %d blocks; isp %d rel\n",
        get_proc_size(udata.u_ptab), get_proc_size_blocks(udata.u_ptab),
        udata.u_isp - PROGBASE);
    for (int i=0; i<NUM_ALLOCATION_BLOCKS; i++)
    {
        const pagemap_entry_t* b = &allocation_map[i];
        void* p = (void*)PROGBASE + i*BLOCKSIZE;
        if (b->block != 0xff)
            kprintf("#%d: slot %d block %d %p\n", i, b->slot, b->block, p);
    }
}
#endif

//--------------------------------------------------------------------+
// pagemapper
//--------------------------------------------------------------------+

/*
On a non single user system provide a pagemap_init which loads a list of
page numbers via pagemap_add. The numbers must *not* include a page 0. If
need be bias or xor with 0x80 or something. At boot you should have 2 or 4
pages mapped (depending upon your kernel), one of which (usually the top) is
common. Add all the pages that are not kernel.
*/
void pagemap_init(void)
{
    //--------------- this was in main ------------------
    // but it *might* sit here well as pagemap_init() it is called
    // very early in fuzix_main()

	// sanity checks
	if ((U_DATA__U_SP_OFFSET != offsetof(struct u_data, u_sp)) ||
		(U_DATA__U_PTAB_OFFSET != offsetof(struct u_data, u_ptab)) ||
		(P_TAB__P_PID_OFFSET != offsetof(struct p_tab, p_pid)) ||
		(P_TAB__P_STATUS_OFFSET != offsetof(struct p_tab, p_status)) ||
		(UDATA_SIZE_ASM != UDATA_SIZE))
	{
		kprintf("U_DATA__U_SP = %d\n", offsetof(struct u_data, u_sp));
		kprintf("U_DATA__U_PTAB = %d\n", offsetof(struct u_data, u_ptab));
		kprintf("P_TAB__P_PID_OFFSET = %d\n", offsetof(struct p_tab, p_pid));
		kprintf("P_TAB__P_STATUS_OFFSET = %d\n", offsetof(struct p_tab, p_status));
		panic("bad offsets");
	}

	// total amount of Kbytes of memory available in the system, including memory used for the kernel and other resources
	ramsize = (SRAM_END - SRAM_BASE) / 1024;

	// the number of Kbytes of memory available for process allocation
	procmem = USERMEM / 1024;
    //--------------- end ------------------

    #ifdef DEBUG
        kprintf("%d blocks of memory\n", NUM_ALLOCATION_BLOCKS);
    #endif
    memset(allocation_map, 0xff, sizeof(allocation_map));
	udata.u_ptab = NULL;
}

uaddr_t pagemap_base(void) {
    return PROGBASE;
}

/*
Allocate memory for the process p.  If no memory is available or the request is
too large to be fulfilled then ENOMEM is returned. Upon success the p->page and
p->page2 values are updated by the memory manager. They can be set however
it wishes in order to indicate the resources allocated.

p->page must not be zero. Zero is used to indicate a process with no memory
currently allocated.
*/
int pagemap_alloc(ptptr p) {
	if (p == udata.u_ptab)
		return 0;

    int blocks = get_proc_size_blocks(p);
    int slot = get_slot(p);
    #ifdef DEBUG
        kprintf("alloc %d, %d blocks\n", get_slot(p), blocks);
    #endif

    for (int i=0; i<blocks; i++)
    {
        pagemap_entry_t* b = find_free_block(p);
        if (!b)
            return ENOMEM;
        b->slot = slot;
        b->block = i;
    }

	p->p_page = 1;
    #ifdef DEBUG
        kprintf("done alloc\n");
        debug_blocks();
    #endif
	return 0;
}

/*
Reallocate the memory for the current process to a new larger size given by
size. Currently this does not need to preserve the existing memory content.
ENOMEM is returned if there is no memory or the request is too large to be
fulfilled. The code and stack values are not currently used but will become
meaningful in future.
Note: size does *not* include udata
*/
int pagemap_realloc(struct exec *hdr, usize_t size) {
    struct p_tab* p = udata.u_ptab;

    uaddr_t oldblocks = get_proc_size_blocks(p);
    int blocks = (int)alignup(size + UDATA_SIZE, BLOCKSIZE) / BLOCKSIZE;
    int slot = get_slot(p);
    #ifdef DEBUG
        kprintf("realloc %d from %d to %d blocks\n", get_slot(udata.u_ptab), oldblocks, blocks);
    #endif
    if (blocks < oldblocks)
    {
        #ifdef DEBUG
            kprintf("shrinking process\n");
        #endif
        for (int i=blocks; i<oldblocks; i++)
        {
            pagemap_entry_t* b = find_block(slot, i);
            b->slot = b->block = 0xff;
        }
    }
    else if (blocks > oldblocks)
    {
        #ifdef DEBUG
            kprintf("growing process\n");
        #endif
        for (int i=oldblocks; i<blocks; i++)
        {
            pagemap_entry_t* b = find_free_block(p);
            if (!b)
                return ENOMEM;
            b->slot = slot;
            b->block = i;
        }
    }

    p->p_top = PROGBASE + blocks*BLOCKSIZE;
    #ifdef DEBUG
        debug_blocks();
    #endif
    contextswitch(p);
	return 0;
}

/*
Free the memory resources assigned to process p.
*/
void pagemap_free(ptptr p) {
    #ifdef DEBUG
        kprintf("free %d\n", get_slot(p));
    #endif
    int slot = get_slot(p);
    for (int i=0; i<NUM_ALLOCATION_BLOCKS; i++)
    {
        pagemap_entry_t* b = &allocation_map[i];
        if (b->slot == slot)
        {
            #ifdef DEBUG
                kprintf("free slot #%d\n", i);
            #endif
            b->slot = b->block = 0xff;
        }
    }

	p->p_page = 0;
}

/*
Reports the number of Kbytes of memory that are currently allocated.
*/
usize_t pagemap_mem_used(void) {
    usize_t count = 0;
    for (int i=0; i<NUM_ALLOCATION_BLOCKS; i++)
    {
        pagemap_entry_t* b = &allocation_map[i];
        if (b->slot != 0xff)
            count++;
    }
    return count * (BLOCKSIZE/1024);
}

//--------------------------------------------------------------------+
// swapper
//--------------------------------------------------------------------+

//
void contextswitch(ptptr p)
{
    #ifdef DEBUG
        kprintf("context switch from %d to %d\n", get_slot(udata.u_ptab), get_slot(p));
    #endif

    if (!p->p_page)
        swapin(p, p->p_page2);

    int slot = get_slot(p);
    int blocks = get_proc_size_blocks(p);
    for (int i=0; i<blocks; i++)
    {
        pagemap_entry_t* b1 = &allocation_map[i];
        int i1 = b1 - allocation_map;
        void* p1 = (void*)PROGBASE + i1*BLOCKSIZE;
        if ((b1->slot != slot) || (b1->block != i))
        {
            pagemap_entry_t* b2 = find_block(slot, i);
            if (!b2)
                panic("missing block");
            int i2 = b2 - allocation_map;
            void* p2 = (void*)PROGBASE + i2*BLOCKSIZE;
            if (b1->slot == 0xff)
            {
                #ifdef DEBUG
                    kprintf("copy #%d to #%d\n", i2, i1);
                #endif
                memcpy(p1, p2, BLOCKSIZE);
            }
            else
            {
                #ifdef DEBUG
                    kprintf("swap #%d and #%d\n", i1, i2);
                #endif
                swap_blocks(p1, p2, BLOCKSIZE);
            }

            pagemap_entry_t t = *b1;
            *b1 = *b2;
            *b2 = t;
        }
    }

    #ifdef DEBUG
        debug_blocks();
    #endif
}

// Copy the current process into a new child slot, and context switch so it's live.
void clonecurrentprocess(ptptr p)
{
    #ifdef DEBUG
        kprintf("clone %d to slot %d\n", get_slot(udata.u_ptab), get_slot(p));
        if (p->p_top != udata.u_ptab->p_top)
            panic("mismatched sizes");
    #endif
    int srcslot = get_slot(udata.u_ptab);
    int destslot = get_slot(p);
    int blocks = get_proc_size_blocks(p);
    for (int i=0; i<blocks; i++)
    {
        pagemap_entry_t* b1 = find_block(srcslot, i);
        pagemap_entry_t* b2 = find_block(destslot, i);
        if (!b1 || !b2)
            panic("missing block");
        int i1 = b1 - allocation_map;
        int i2 = b2 - allocation_map;
        void* p1 = (void*)PROGBASE + i1*BLOCKSIZE;
        void* p2 = (void*)PROGBASE + i2*BLOCKSIZE;
        #ifdef DEBUG
            kprintf("copy #%d to #%d (%p to %p)\n", i1, i2, p1, p2);
        #endif
        memcpy(p2, p1, BLOCKSIZE);

        pagemap_entry_t t = *b1;
        *b1 = *b2;
        *b2 = t;
    }
    #ifdef DEBUG
        kprintf("end clone\n");
    #endif
}

// Swap ourself out
int swapout(ptptr p)
{
#ifdef DEBUG
	kprintf("swapping out %d (%d)\n", get_slot(p), p->p_pid);
#endif

	uint16_t page = p->p_page;
	if (!page)
		panic(PANIC_ALREADYSWAP);
    if (SWAPDEV == 0xffff)
        return ENOMEM;

	/* Are we out of swap ? */
	int16_t map = swapmap_alloc();
	if (map == -1)
		return ENOMEM;

	uint16_t swaparea = map * SWAP_SIZE;

    int slot = get_slot(p);
    int blocks = get_proc_size_blocks(p);
    for (int i=0; i<blocks; i++)
    {
        pagemap_entry_t* b = find_block(slot, i);
        int blockindex = b - allocation_map;
        void* p = (void*)PROGBASE + blockindex*BLOCKSIZE;

        swapwrite(SWAPDEV, swaparea + (i*(BLOCKSIZE>>BLKSHIFT)),
            BLOCKSIZE, (uaddr_t)p, 1);

        b->slot = b->block = 0xff;
    }

	p->p_page = 0;
	p->p_page2 = map;
	return 0;
}

// Swap ourself in: must be on the swap stack when we do this
void swapin(ptptr p, uint16_t map)
{
    uint16_t swaparea = map * SWAP_SIZE;

    int slot = get_slot(p);
    int blocks = get_proc_size_blocks(p);
    for (int i=0; i<blocks; i++)
    {
        pagemap_entry_t* b = find_free_block(p);
        int blockindex = b - allocation_map;
        void* p = (void*)PROGBASE + blockindex*BLOCKSIZE;

        swapread(SWAPDEV, swaparea + (i*(BLOCKSIZE>>BLKSHIFT)), BLOCKSIZE, (uaddr_t)p, 1);

        b->slot = slot;
        b->block = i;
    }

    p->p_page = 1;
    p->p_page2 = 0;
}
