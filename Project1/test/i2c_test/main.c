#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#include "user_i2c.h"
#include "apds9301.h"
#include "tmp1021.h"

#define TEMP_TASK
#define LIGHT_TASK
#define LIGHT_TASK_ALERT

#define RETRY_MAX (3)

int32_t tmp1021_init(int32_t *dev_fp);
int32_t apds9301_init(int32_t *dev_fp);
float tmp_raw_to_temperature(int16_t raw, int32_t format);
float apds_raw_to_lux(int16_t ch0, int16_t ch1);

#ifdef LIGHT_TASK_ALERT
void apds_irq_handler(int signo)
{
  if(signo == SIGIO)
  {
    /* Clear the apds interrupt assertion */

    printf("light alert!\n");
  }
  return;
}
#endif

/* Missing:
 * - Hearbeat report
 * - Response to external request
 * - Temperature alert
 * - Enqueue temperature read to msg queue
 */
#ifdef TEMP_TASK
void * task_temp(void * param)
{
  int32_t tmp_handle;
  int16_t tmp;
  float temperature;
  uint8_t retry = 0;
  /* Set up the tmp1021 sensor by default */
  if(tmp1021_init(&tmp_handle)!=0)
  {
    // Error log
    exit(EXIT_FAILURE);
  }

  /* Periodic temperature read */
  while(1)
  {
    /* Processing all msg in the queue */
    /* - Heartbeat request from main
       - External request */

    /* Read temperature raw data from the sensor */
    if(i2c_read_word_mutex(tmp_handle, TEMP_REG, &tmp)!=0)
    {
      /* If the read fails 3 times consecutively, break the read loop */
      if(retry == 3)
      {
        /* Enqueue error message onto the queue */
        break;
      }
      else
        retry++;
    }
    else
    {
      retry = 0;
      /* Convert digital data format to temperature */
      temperature = tmp_raw_to_temperature(tmp, CELSIUS_FORMAT);
      /* Enqueue the temperature onto the msg queue */
      printf("Temperature: %.2f\n", temperature);
    }
    /* Sleep */
    sleep(1);
  }

  /* If ever breaks the loop, clean up */
  i2c_disconnect_mutex(tmp_handle);
  /* Enqueue task close message */

}
#endif

/*  Missing
 *  - Heartbeat report
 *  - Response to external request
 *  - Interrupt setup
 *  - Enqueue msg to queue
 */
#ifdef LIGHT_TASK
void * task_light(void * param)
{
  int32_t apds_handle;
  int32_t apds_irq_handle;
  struct sigaction apds_irq_action;
  int16_t ch0_data, ch1_data;
  float light;
  uint8_t retry = 0;
  /* Set up the light sensor */
  if(apds9301_init(&apds_handle)!=0)
  {
    // Error log
    exit(EXIT_FAILURE);
  }

#ifdef LIGHT_TASK_ALERT
  /* Set up the pin interrupt for handling light alert */
  memset(&apds_irq_action, 0, sizeof(apds_irq_action));
  apds_irq_action.sa_handler = apds_irq_handler;
  apds_irq_action.sa_flags = 0;
  if(sigaction(SIGIO, &apds_irq_action, NULL)!=0)
  {
    // Error log
    exit(EXIT_FAILURE);
  }
  if((apds_irq_handle=open("/dev/gpio_apds_int", O_RDWR))<0)
  {
    // Error log
    exit(EXIT_FAILURE);
  }
  if(fcntl(apds_irq_handle, F_SETOWN, getpid())==-1)
  {
      // Error log
      exit(EXIT_FAILURE);
  }
  if(fcntl(apds_irq_handle, F_SETFL,
           fcntl(apds_irq_handle, F_GETFL) | FASYNC) == -1)
  {
    // Error log
    exit(EXIT_FAILURE);
  }
#endif

  while(1)
  {
    /* Enqueue state changes to the msg queue */

    /* Get light level in lux */
    if((i2c_read_word_mutex(apds_handle, CMD | WORD | DATA0_REG,
                            &ch0_data)!=0) ||
       (i2c_read_word_mutex(apds_handle, CMD | WORD | DATA1_REG,
                            &ch1_data)!=0))
    {
      /* If the read fails 3 times consecutively, break the read loop */
      if(retry == RETRY_MAX)
      {
        /* Enqueue error message onto the queue */
        break;
      }
      else
        retry++;
    }
    else
    {
      retry = 0;
      light = apds_raw_to_lux(ch0_data, ch1_data);
      printf("Light:%.2f\n", light);
    }
    /* sleep */
    sleep(1);
  }

  /* If ever breaks the loop, clean up */
  i2c_disconnect_mutex(apds_handle);
}
#endif

int32_t tmp1021_init(int32_t *dev_fp)
{
  int32_t ret = 0;
  /* Connect the tmp1021 to the i2c interface */
  if(i2c_connect_mutex(dev_fp, TMP1021_ADDR)!=0)
    ret = -1;

  return ret;
}

int32_t apds9301_init(int32_t *dev_fp)
{
  int32_t ret = 0;
  /* Connect the apds9301 to the i2c interface */
  if(i2c_connect_mutex(dev_fp, APDS9301_ADDR)!=0)
    ret = -1;

  /* Power on the sensor */
  if(i2c_write_byte_mutex(*dev_fp, CMD | CONTROL_REG, POWER_ON)!=0)
    ret = -1;

  /* Configure the alert thresholds */
#ifdef LIGHT_TASK_ALERT
  if(i2c_write_word_mutex(*dev_fp, CMD | WORD | THRESHLOW_REG,
                          (uint16_t)DEFAULT_THRESHLOW_VALUE)!=0)
    ret = -1;
  if(i2c_write_word_mutex(*dev_fp, CMD | WORD | THRESHHIGH_REG,
                          (uint16_t)DEFAULT_THRESHHIGH_VALUE)!=0)
    ret = -1;
  if(i2c_write_byte_mutex(*dev_fp, CMD | INTERRUPT_REG,
                          (uint8_t)DEFAULT_INTERRUPT_SETTING))
    ret = -1;
#endif

  return ret;
}

float tmp_raw_to_temperature(int16_t raw, int32_t format)
{
  float temperature;
  raw = (raw & 0xFFF0) >> 4;
  switch (format)
  {
    case CELSIUS_FORMAT:
      temperature  = raw * 0.0625;
      break;
    case FAHRENHEIT_FORMAT:
      temperature = raw * 0.1125 +32;
      break;
  }
  return temperature;
}

float apds_raw_to_lux(int16_t ch0, int16_t ch1)
{
  float lux;
  float ratio = (float)ch1/ch0;
  if ((0.0f<ratio)&&(ratio<=0.5f))
    lux = 0.0304f*ch0-0.062f*ch0*pow(ratio,1.4f);
  else if (ratio<=0.61f)
    lux = 0.0224f*ch0-0.031f*ch1;
  else if (ratio<=0.8f)
    lux = 0.0128f*ch0-0.0153f*ch1;
  else if (ratio<=1.3f)
    lux = 0.00146f*ch0-0.00112f*ch1;
  else
    lux = 0.0f;
  return lux;
}

int main(int argc, char const *argv[])
{
  pthread_t thread1, thread2;
#ifdef TEMP_TASK
  if(pthread_create(&thread1, NULL, task_temp, NULL))
  {
    perror("Failed to create the temperature task.");
    exit(EXIT_FAILURE);
  }
#endif
#ifdef LIGHT_TASK
  if(pthread_create(&thread2, NULL, task_light, NULL))
  {
    perror("Failed to create the light task.");
    exit(EXIT_FAILURE);
  }
#endif
#ifdef TEMP_TASK
  pthread_join(thread1, NULL);
#endif
#ifdef LIGHT_TASK
  pthread_join(thread2, NULL);
#endif
  exit(EXIT_SUCCESS);
}
