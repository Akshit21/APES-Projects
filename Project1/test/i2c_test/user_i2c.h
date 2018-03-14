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
                            int8_t *rbuff);
int32_t i2c_read_word_mutex(int32_t dev_fp, uint8_t reg_addr,
                            int16_t *rbuff);
int32_t i2c_connect_mutex(int32_t *dev_fp, int32_t dev_addr);
void i2c_disconnect_mutex(int32_t dev_fp);


#endif
