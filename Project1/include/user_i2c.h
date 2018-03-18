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

/**
* @brief Function to write a byte via i2c
*
* @param dev_fp - a handle to device
*        reg_addr - the address of the register to wirte
*        value - the value to right
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_write_reg(int32_t dev_fp, uint8_t reg_addr, uint8_t value);

/**
* @brief Function to read a byte via i2c
*
* @param dev_fp - a handle to device
*        reg_addr - the address of the register to read from
*        rbuff -  pointer to variable to store the read
*        num - number of bytes to read
*
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_read_reg(int32_t dev_fp, uint8_t reg_addr, int8_t *rbuff, uint32_t num);

/**
* @brief Function to connect a i2c device
*
* @param dev_fp - pointer to a variable to store the device handle
*        dev_addr - the address of the device
*
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_connect(int32_t *dev_fp, int32_t dev_addr);

/**
* @brief Function to disconnect a i2c device
*
* @param dev_fp - a handle to device
*
*/
void i2c_disconnect(int32_t dev_fp);

/**
* @brief Function to write a byte via i2c with mutex protection
*
* @param dev_fp - a handle to device
*        reg_addr - the address of the register to wirte
*        value - the value to right
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_write_byte_mutex(int32_t dev_fp, uint8_t reg_addr, uint8_t value);

/**
* @brief Function to write a word via i2c with mutex protection
*
* @param dev_fp - a handle to device
*        reg_addr - the address of the register to wirte
*        value - the value to right
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_write_word_mutex(int32_t dev_fp, uint8_t reg_addr, uint16_t value);

/**
* @brief Function to read a byte via i2c with mutex protection
*
* @param dev_fp - a handle to device
*        reg_addr - the address of the register to read from
*        rbuff -  pointer to variable to store the read
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_read_byte_mutex(int32_t dev_fp, uint8_t reg_addr, uint8_t *rbuff);

/**
* @brief Function to read a word via i2c with mutex protection
*
* @param dev_fp - a handle to device
*        reg_addr - the address of the register to read from
*        rbuff -  pointer to variable to store the read
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_read_word_mutex(int32_t dev_fp, uint8_t reg_addr, uint16_t *rbuff);

/**
* @brief Function to connect a i2c device with mutext protection
*
* @param dev_fp - pointer to a variable to store the device handle
*        dev_addr - the address of the device
*
* @return 0 - Success | 1 - Failure
*/
int32_t i2c_connect_mutex(int32_t *dev_fp, int32_t dev_addr);

/**
* @brief Function to disconnect a i2c device with mutex protection
*
* @param dev_fp - a handle to device
*
*/
void i2c_disconnect_mutex(int32_t dev_fp);


#endif
