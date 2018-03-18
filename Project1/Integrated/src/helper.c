#include "project.h"

/* Function to setup timer and sigevent */
int32_t timer_setup(timer_t *timer_id, uint32_t ms, void (*handler))
{
	struct itimerspec ts;
	struct sigevent se;

	/* Set the sigevent struct to handle the timer signal */
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = timer_id;
	se.sigev_notify_function = handler;
	se.sigev_notify_attributes = NULL;

	/* Set up the timer */
	ts.it_value.tv_sec = ms / 1000;
	ts.it_value.tv_nsec = (ms % 1000) * 1000;
	ts.it_interval.tv_sec = ms / 1000;
	ts.it_interval.tv_nsec = (ms % 1000) * 1000;
	if(timer_create(CLOCK_REALTIME, &se, timer_id)==-1)
		return -1;
	if(timer_settime(*timer_id, 0, &ts, 0)==-1)
		return -1;
	return 0;
}
