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
* @date : 03/04/2018
*
* @file : apds9301.h
* @brief : This header file provides register defination of light sensor
***************************************************************************************************/
#ifndef _APDS9301_H__
#define _APDS9301_H__

#include <stdint.h>
#include "user_i2c.h"

#define APDS9301_ADDR (0x39)

#define CONTROL_REG (0x00)
#define TIMING_REG  (0x01)
#define THRESHLOW_REG (0x02)
#define THRESHLOWLOW_REG  (0x02)
#define THRESHLOWHIGH_REG (0x03)
#define THRESHHIGH_REG  (0x04)
#define THRESHHIGHLOW_REG (0x04)
#define THRESHHIGHHIGH_REG  (0x05)
#define INTERRUPT_REG (0x06)
#define ID_REG  (0x0A)
#define DATA0_REG   (0x0C)
#define DATA0LOW_REG  (0x0C)
#define DATA0HIGH_REG (0x0D)
#define DATA1_REG (0x0E)
#define DATA1LOW_REG (0x0E)
#define DATA1HIGH_REG (0x0F)

#define POWER_ON  (0x03)
#define POWER_OFF (0x00)

#define CMD (0x80)
#define CLEAR (0x40)
#define WORD (0x20)

#define INTR_ENABLE (0x01)
#define INTR_DISABLE  (0x00)
#define PERSIST_ALWAYS  (0x00)
#define PERSIST_OUT_OF_RANGE  (0x01)
#define PERSIST_INTEGRATION_TIME_PERIODS_2  (0x02)
#define PERSIST_INTEGRATION_TIME_PERIODS_3  (0x03)
#define PERSIST_INTEGRATION_TIME_PERIODS_4  (0x04)
#define PERSIST_INTEGRATION_TIME_PERIODS_5  (0x05)
#define PERSIST_INTEGRATION_TIME_PERIODS_6  (0x06)
#define PERSIST_INTEGRATION_TIME_PERIODS_7  (0x07)
#define PERSIST_INTEGRATION_TIME_PERIODS_8  (0x08)
#define PERSIST_INTEGRATION_TIME_PERIODS_9  (0x09)
#define PERSIST_INTEGRATION_TIME_PERIODS_10 (0x0A)
#define PERSIST_INTEGRATION_TIME_PERIODS_11 (0x0B)
#define PERSIST_INTEGRATION_TIME_PERIODS_12 (0x0C)
#define PERSIST_INTEGRATION_TIME_PERIODS_13 (0x0D)
#define PERSIST_INTEGRATION_TIME_PERIODS_14 (0x0E)
#define PERSIST_INTEGRATION_TIME_PERIODS_15 (0x0F)

#define DEFAULT_THRESH_VALUE (500)
#define THRESH_MAX  (0xff00)
#define THRESH_MIN  (0)
#define DEFAULT_INTERRUPT_SETTING (0x11)

#define LIGHT_STATE_DAY (0)
#define LIGHT_STATE_NIGHT  (1)
#endif
