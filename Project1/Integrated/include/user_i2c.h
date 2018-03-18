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
* @file : user_i2c.h
* @brief : This header file provides an abstraction of I2C interface.
***************************************************************************************************/
#ifndef _USER_I2C_H__
#define _USER_I2C_H__

#include <stdint.h>
#include <pthread.h>

typedef struct i2c_lock
{
    pthread_mutex_t mutex;
    int refcnt;
}i2c_lock_t;

int32_t i2c_write_reg(int32_t dev_fp, uint8_t reg_addr,
                      uint8_t value);
int32_t i2c_read_reg(int32_t dev_fp, uint8_t reg_addr,
                     int8_t *rbuff, uint32_t num);
int32_t i2c_connect(int32_t *dev_fp, int32_t dev_addr);
void i2c_disconnect(int32_t dev_fp);

int32_t i2c_write_byte_mutex(int32_t dev_fp, uint8_t reg_addr,
                            uint8_t value);
int32_t i2c_write_word_mutex(int32_t dev_fp, uint8_t reg_addr,
                            uint16_t value);
int32_t i2c_read_byte_mutex(int32_t dev_fp, uint8_t reg_addr,
                            uint8_t *rbuff);
int32_t i2c_read_word_mutex(int32_t dev_fp, uint8_t reg_addr,
                            uint16_t *rbuff);
int32_t i2c_connect_mutex(int32_t *dev_fp, int32_t dev_addr);
void i2c_disconnect_mutex(int32_t dev_fp);


#endif
