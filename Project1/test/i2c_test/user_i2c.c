#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include "user_i2c.h"

pthread_mutex_t i2c_lock;

int32_t i2c_write_reg(int32_t fp, uint8_t reg_addr, int8_t value)
{
  uint8_t write_buff[2];
  write_buff[0] = reg_addr;
  write_buff[1] = value;
  if(write(fp, write_buff, 2)!=2)
  {
    perror("Failed to write to the device.\n");
    return -1;
  }
  return 0;
}

int32_t i2c_read_reg(int32_t fp, uint8_t reg_addr, int8_t *rbuff, uint32_t num)
{
  reg_addr |= 0x80;
  if(write(fp, &reg_addr, 1)!=1)
  {
    perror("Failed to read the register address.\n");
    return -1;
  }
  if(read(fp, rbuff, num)!=num)
  {
    perror("Failed to read from device.\n");
    return -1;
  }
  return 0;
}

int32_t i2c_connect(const char * i2c_index, int32_t dev_addr)
{
  int32_t dev_fp;
  if((dev_fp=open(i2c_index, O_RDWR))<0)
  {
    perror("Failed to open the bus.\n");
    return -1;
  }
  if(ioctl(dev_fp, I2C_SLAVE, dev_addr)<0)
  {
    perror("Failed to connect to the sensor AV02.\n");
    return -1;
  }
  return dev_fp;
}

void i2c_disconnect(int32_t fp)
{
  close(fp);
}

int32_t i2c_write_reg_mutex(int32_t fp, uint8_t reg_addr, int8_t value)
{
  int32_t rc;
  uint8_t write_buff[2];
  write_buff[0] = reg_addr;
  write_buff[1] = value;
  pthread_mutex_lock(&i2c_lock);
  rc = write(fp, write_buff, 2);
  pthread_mutex_unlock(&i2c_lock);
  if(rc!=2)
  {
    perror("Failed to write to the device.\n");
    return -1;
  }
  return 0;
}

int32_t i2c_read_reg_mutex(int32_t fp, uint8_t reg_addr,
                           int8_t *rbuff, uint32_t num)
{
  reg_addr |= 0x80;
  pthread_mutex_lock(&i2c_lock);
  if(write(fp, &reg_addr, 1)!=1)
  {
    pthread_mutex_unlock(&i2c_lock);
    perror("Failed to read the register address.\n");
    return -1;
  }
  if(read(fp, rbuff, num)!=num)
  {
    pthread_mutex_unlock(&i2c_lock);
    perror("Failed to read from device.\n");
    return -1;
  }
  pthread_mutex_unlock(&i2c_lock);
  return 0;
}

int32_t i2c_connect_mutex(const char * i2c_index, int32_t dev_addr)
{
  int32_t dev_fp;
  if((dev_fp=open(i2c_index, O_RDWR))<0)
  {
    perror("Failed to open the bus.\n");
    return -1;
  }
  if(ioctl(dev_fp, I2C_SLAVE, dev_addr)<0)
  {
    perror("Failed to connect to the sensor AV02.\n");
    return -1;
  }
  if(pthread_mutex_init(&i2c_lock, NULL))
  {
    perror("Failed to initialize mutex lock for i2c.\n");
    return -1;
  }
  return dev_fp;
}

void i2c_disconnect_mutex(int32_t fp)
{
  pthread_mutex_destroy(&i2c_lock);
  close(fp);
}
