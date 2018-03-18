/***************************************************************************************************
* Copyright (C) 2017 by Akshit Shah, Shuting Guo
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Akshit Shah, Shuting Guo, Prof Alex Fosdick and the University of Colorado are
* not liable for any misuse of this material.
***************************************************************************************************/
/***************************************************************************************************
* @author : Akshit Shah, Shuting Guo
* @date : 02/22/2018
*
* @file : .helper.h
* @brief : This header file provides an func declaration of timer setup
***************************************************************************************************/
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
