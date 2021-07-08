#if MANGLED == 1
	#define panic f_panic
	#define _read f_read
	#define _write f_write
	#define _sbrk f_sbrk
	#define queue_t f_queue
#else
	#undef panic
	#undef _read
	#undef _write
	#undef _sbrk
	#undef queue_t
#endif

#undef MANGLED
