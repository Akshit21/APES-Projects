#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <errno.h>
#include <semaphore.h>

#include "user_i2c.h"
#include "apds9301.h"
#include "tmp1021.h"

#define TEMP_TASK
#define LIGHT_TASK
#define LIGHT_TASK_ALERT

#define TMP_PERIOD	(2000)
#define RETRY_MAX (3)

pthread_mutex_t tmp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tmp_cond = PTHREAD_COND_INITIALIZER;
sem_t apds_sem;

int32_t tmp1021_init(int32_t *dev_fp);
int32_t apds9301_init(int32_t *dev_fp);
float tmp_raw_to_temperature(int16_t raw, int32_t format);
float apds_raw_to_lux(int16_t ch0, int16_t ch1);
int32_t timer_setup(uint32_t ms, void (*handler));

#ifdef LIGHT_TASK_ALERT
void apds_irq_handler(int signo)
{
  if(signo == SIGIO)
  {
    /* Clear the apds interrupt assertion */
  	sem_post(&apds_sem);
  }
}
#endif

void tmp_timer_handler(union sigval arg)
{
  pthread_mutex_lock(&tmp_mutex);
	pthread_cond_signal(&tmp_cond);
  pthread_mutex_unlock(&tmp_mutex);
}


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
  timer_t timer_id;
  float temperature;
  uint8_t retry = 0;
 /* Set up the tmp1021 sensor by default */
  if(tmp1021_init(&tmp_handle)!=0)
  {
    // Error log
    return;
  }

  /* Set up a periodic timer */
  if((timer_id = timer_setup(TMP_PERIOD, tmp_timer_handler))==-1)
  {
  	// error;
	return;
  }

  /* Periodic temperature read */
  while(1)
  {
    /* Processing all msg in the queue */
    /* - Heartbeat request from main
       - External request */
	pthread_cond_wait(&tmp_cond, &tmp_mutex);
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
  }

  /* If ever breaks the loop, clean up */
  timer_delete(timer_id);
  i2c_disconnect_mutex(tmp_handle);
  /* Enqueue task close message */
  return;
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
  uint8_t op_flag = 0, retry = 0;
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
  if((apds_irq_handle=open("/dev/gpio_int", O_RDWR))<0)
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

  /* Initialzie a semephore for ligth state change event */
  sem_init(&apds_sem, 0, 0);
  while(1)
  {
    /* Enqueue state changes to the msg queue */

    /* Wait for light state change event */
    sem_wait(&apds_sem);
    /* Update light state */
    while((retry<=RETRY_MAX)&&(op_flag==0))
    {
      if((i2c_read_word_mutex(apds_handle, CMD | WORD | DATA0_REG, &ch0_data)!=0) ||
         (i2c_read_word_mutex(apds_handle, CMD | WORD | DATA1_REG, &ch1_data)!=0))
      {
        retry++;
      }
      else
      {
        retry = 0;
        op_flag = 1;
      }
    }
    /* Failed to read from sensors consecutively */
    if(op_flag==0)
    {
      //error
      return;
    }

    op_flag = 0;
    light_state = ch0_data > DEFAULT_THRESHHIGH_VALUE ? EXPOSED :
                  ch0_data < DEFAULT_THRESHLOW_VALUE ? DARK : NORMAL;

    /* Enqueue error message onto the queue */

    //light = apds_raw_to_lux(ch0_data, ch1_data);
    //printf("Light:%.2f\n", light);
  }

  /* If ever breaks the loop, clean up */
	close(adps_irq_handle);
	pthread_cond_destroy(&cond, NULL);
	pthread_mutex_destroy(&mutex, NULL);
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
  raw = (int16_t)((raw&0xf000)>>12) | ((raw&0x00ff)<<4);
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

int32_t timer_setup(uint32_t ms, void (*handler))
{
	timer_t timer_id;
	struct itimerspec ts;
	struct sigevent se;

	/* Set the sigevent struct to handle the timer signal */
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timer_id;
	se.sigev_notify_function = handler;
	se.sigev_notify_attributes = NULL;

	/* Set up the timer */
	ts.it_value.tv_sec = ms / 1000;
	ts.it_value.tv_nsec = (ms % 1000) * 1000;
	ts.it_interval.tv_sec = ms / 1000;
	ts.it_interval.tv_nsec = (ms % 1000) * 1000;
	if(timer_create(CLOCK_REALTIME, &se, &timer_id)==-1)
		return -1;
	if(timer_settime(timer_id, 0, &ts, 0)==-1)
		return -1;
	return timer_id;
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
