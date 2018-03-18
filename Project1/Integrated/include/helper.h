#ifndef _HELPER_H__
#define _HELPER_H__

/**
* @brief Function to setup timer for signalevent`
*
* @param timer_id Struct timer id
* @param ms time in milliseconds for the interrupt
* @param handler Void pointer to the callback routine
*
* @return int32_t SUCCES/ERROR
*/
int32_t timer_setup(timer_t *timer_id, uint32_t ms, void (*handler));
#endif
