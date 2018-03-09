#ifndef _USER_I2C_H__
#define _USER_I2C_H__

#include <stdint.h>

int32_t i2c_write_reg(int32_t fp, uint8_t reg_addr, uint8_t value);
int32_t i2c_read_reg(int32_t fp,uint8_t reg_addr,
                     uint8_t *rbuff, uint32_t num);
int32_t i2c_connect(const char* i2c_index, int32_t dev_addr);
void i2c_disconnect(int32_t fp);

int32_t i2c_write_reg_mutex(int32_t fp, uint8_t reg_addr, uint8_t value);
int32_t i2c_read_reg_mutex(int32_t fp,uint8_t reg_addr,
                           uint8_t *rbuff, uint32_t num);
int32_t i2c_connect_mutex(const char* i2c_index, int32_t dev_addr);
void i2c_disconnect_mutex(int32_t fp);


#endif
