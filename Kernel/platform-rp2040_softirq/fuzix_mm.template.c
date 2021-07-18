#include "fuzix_platform.h"

/*
CONFIG_FLAT
    The flat memory manager is designed to be used with 32bit systems that have a
    single large flat address space in which all processes live. This covers
    most platforms using processors such as the Motorola 68000. Memory is
    handed out by an allocator into the flat space. Swapping is not supported.
    The fork() system call and Posix semantics are maintained by block copying
    when needed.
*/

//--------------------------------------------------------------------+
// base befines and methods
//--------------------------------------------------------------------+

// the base address at which programs begin (usually 0)
#define PROGBASE
// the base address at which programs are loaded
#define PROGLOAD
// the first byte above the program space
#define PROGTOP

/*
The total amount of memory available in the system, including memory used for
the kernel and other resources. This should reflect the way the platform
normal reports total memory. It is used solely to provide feedback to the user
on their system configuration. It should thus match the values reported by
firmware or other convention.
*/
usize_t ramsize;

/*
Set by the platform the number of Kbytes of memory available for process
allocation. This is used by tools such as 'free' to help report memory
utilization.
*/
usize_t procmem;

/*
Allocate memory for the process p.  If no memory is available or the request is
too large to be fulfilled then ENOMEM is returned. Upon success the p->page and
p->page2 values are updated by the memory manager. They can be set however
it wishes in order to indicate the resources allocated.

p->page must not be zero. Zero is used to indicate a process with no memory
currently allocated.
*/
int pagemap_alloc(ptptr p) {}

/*
Reallocate the memory for the current process to a new larger size given by
size. Currently this does not need to preserve the existing memory content.
ENOMEM is returned if there is no memory or the request is too large to be
fulfilled. The code and stack values are not currently used but will become
meaningful in future.
*/
int pagemap_realloc(usize_t code, usize_t size, usize_t stack) {}

/*
Free the memory resources assigned to process p.
*/
int pagemap_free(ptptr p) {}

/*
Reports the number of Kbytes of memory that are currently allocated.
*/
usize_t pagemap_mem_used(void) {}

//--------------------------------------------------------------------+
// flat memory manager specific methods
//--------------------------------------------------------------------+

/*
Switch the memory maps around so that the memory of process p appears
mapped at the correct address range. This is normally used internally by the
task switching logic.
*/
void pagemap_switch(ptptr p, int death) {}

/*
Returns the base address at which the program was loaded. This helper is
used internally by the execve syscall code on 32bit systems.
*/
uaddr_t pagemap_base(void) {}

/*
arg_t _memalloc(void) {}
arg_t _memfree(void) {}

These functions are an implementation of the additional user space memory
allocator functions on flat memory model systems. They are directly hooked
into the system call tables.

An underlying kernel memory allocator must also be used. This is normally
the kernel malloc. At minimum the platform needs to have called

void kmemaddblk(void *base, size_t size)

to add a block of memory size bytes long from the address base to the memory
pool available for kernel allocation. This pool is used both for kernel
allocations via kmalloc and kfree, as well as the process space for
executables in flat memory space. The passed base address must be aligned to
the alignment required by the processor and allocator.
*/
