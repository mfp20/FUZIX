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
		softirq_t irq;
		// TODO use the unused byte
		if (!mk_softirq(&irq, DEV_ID_TIMER, SIG_ID_TICK, 0, NULL))
		{
			// TODO out of memory error -> tick lost
			return false;
		}
		// queue softirq
		if (!queue_try_add(&softirq_out_q, &irq))
		{
			// TODO queue full error -> lag -> tick lost
			return false;
		}
	}

	return true;
}

void ticker_init(void) {
	add_repeating_timer_us((1000000 / TICKSPERSEC), systick_timer_handler, NULL, &systick_timer);
}