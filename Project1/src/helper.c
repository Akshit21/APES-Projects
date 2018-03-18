/**************************************************************************************************
* Copyright (C) 2017 by Akshit Shah, Shuting Guo
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Akshit Shah, Shuting Guo, Prof. Alex Fosdick and the University of Colorado are
* not liable for any misuse of this material.
***************************************************************************************************/
/***************************************************************************************************
* @author : Akshit Shah, Shuting Guo
* @date : 03/5/2018
*
* @file : helper.c
* @brief : APIs for timer setup for signal event
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/

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
