#include "platform.h"

static repeating_timer_t systick_timer;

static bool systick_timer_handler(repeating_timer_t *rt)
{
	if (fuzix_ready && queue_is_empty(&softirq_out_q))
	{
		timer_interrupt();
	}
	else
	{
		softirq_out(DEV_ID_TIMER, SIG_ID_TICK, 0, NULL);
	}

	return true;
}

void virtual_ticker_init(void) {
	add_repeating_timer_us((1000000 / TICKSPERSEC), systick_timer_handler, NULL, &systick_timer);
}
