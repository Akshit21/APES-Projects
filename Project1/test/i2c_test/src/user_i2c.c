#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include "user_i2c.h"

i2c_lock_t lock;

int32_t i2c_write_byte(int32_t dev_fp, uint8_t reg_addr,
                      uint8_t value)
{
  int32_t ret = 0;
  uint8_t write_buff[2];
  write_buff[0] = reg_addr;
  write_buff[1] = value;
  if(write(dev_fp, write_buff, 2)!=2)
  {
    perror("Failed to write to the device.\n");
    ret = -1;
  }
  return ret;
}

int32_t i2c_read_byte(int32_t dev_fp, uint8_t reg_addr,
                     uint8_t *rbuff, uint32_t num)
{
  int32_t ret;
  reg_addr |= 0x80;
  if(write(dev_fp, &reg_addr, 1) != 1)
  {
    perror("Failed to read the register address.\n");
    ret = -1;
  }
  if(read(dev_fp, rbuff, num)!=num)
  {
    perror("Failed to read from device.\n");
    ret = -1;
  }
  return ret;
}

int32_t i2c_connect(int32_t *dev_fp, int32_t dev_addr)
{
  int32_t ret = 0;
  if((*dev_fp=open("/dev/i2c-2", O_RDWR))<0)
  {
    perror("Failed to open the bus.\n");
    ret = -1;
  }
  if(ioctl(*dev_fp, I2C_SLAVE, dev_addr)<0)
  {
    perror("Failed to connect to the sensor AV02.\n");
    ret = -1;
  }
  return ret;
}

void i2c_disconnect(int32_t dev_fp)
{
  close(dev_fp);
}

int32_t i2c_write_byte_mutex(int32_t dev_fp, uint8_t reg_addr,
                            uint8_t value)
{
  int32_t ret = 0;
  // uint8_t write_buff[2];
  // write_buff[0] = reg_addr;
  // write_buff[1] = value;
  pthread_mutex_lock(&lock.mutex);
  //if(write(dev_fp, write_buff, 2) != 2)
  if(i2c_smbus_write_byte_data(dev_fp, reg_addr, value)==-1)
  {
    perror("Failed to write to the device.\n");
    ret = -1;
  }
  pthread_mutex_unlock(&lock.mutex);
  return ret;
}

int32_t i2c_write_word_mutex(int32_t dev_fp, uint8_t reg_addr,
                             uint16_t value)
{
    int32_t ret = 0;
    pthread_mutex_lock(&lock.mutex);
    if(i2c_smbus_write_word_data(dev_fp, reg_addr, value)==-1)
    {
      perror("Failed to write to the device.\n");
      ret = -1;
    }
    pthread_mutex_unlock(&lock.mutex);
    return ret;
}

int32_t i2c_read_byte_mutex(int32_t dev_fp, uint8_t reg_addr,
                           uint8_t *rbuff)
{
  int32_t ret = 0;
  pthread_mutex_lock(&lock.mutex);
  // if(write(dev_fp, &reg_addr, 1)!=1)
  // {
  //   perror("Failed to write the read register address.\n");
  //   ret = -1;
  // }
  // if(read(dev_fp, rbuff, num)!=num)
  // {
  //   perror("Failed to read from device.\n");
  //   ret = -1;
  // }
  if((*rbuff=i2c_smbus_read_byte_data(dev_fp, reg_addr))==-1)
  {
    perror("Failed to read from device.\n");
    ret = -1;
  }
  pthread_mutex_unlock(&lock.mutex);
  return ret;
}

int32_t i2c_read_word_mutex(int32_t dev_fp, uint8_t reg_addr,
                            uint16_t * rbuff)
{
  int32_t ret = 0;
  pthread_mutex_lock(&lock.mutex);
  if((*rbuff=i2c_smbus_read_word_data(dev_fp, reg_addr))==-1)
  {
    perror("Failed to read from device.\n");
    ret = -1;
  }
  pthread_mutex_unlock(&lock.mutex);
  return ret;
}

int32_t i2c_connect_mutex(int32_t *dev_fp, int32_t dev_addr)
{
  int32_t ret = 0;
  lock.refcnt ++;
  if(lock.refcnt == 1)
  {
    if(pthread_mutex_init(&lock.mutex, NULL))
    {
      perror("Failed to initialize mutex lock for i2c.\n");
      lock.refcnt --;
      ret = -1;
    }
  }
  if((*dev_fp=open("/dev/i2c-2", O_RDWR))<0)
  {
    perror("Failed to open the bus.\n");
    ret = -1;
  }
  if(ioctl(*dev_fp, I2C_SLAVE, dev_addr)<0)
  {
    perror("Failed to connect to the device.\n");
    ret = -1;
  }
  return ret;
}

void i2c_disconnect_mutex(int32_t dev_fp)
{
  close(dev_fp);
  if(lock.refcnt > 0)
  {
    pthread_mutex_lock(&lock.mutex);
    if(--lock.refcnt == 0)
    {
      pthread_mutex_unlock(&lock.mutex);
      pthread_mutex_destroy(&lock.mutex);
    }
    else
    {
      pthread_mutex_unlock(&lock.mutex);
    }
  }
}
