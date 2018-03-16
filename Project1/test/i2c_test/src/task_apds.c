
#include "project.h"

sem_t apds_sem;

uint32_t light_state;


static int32_t apds_update_state(int32_t dev_fp, uint32_t state);
static float apds_raw_to_lux(int16_t ch0, int16_t ch1);

#ifdef LIGHT_TASK_ALERT
void apds_irq_handler(int signo)
{
  if(signo == SIGIO)
  {
    printf("light alert\n");
    /* Clear the apds interrupt assertion */
  	sem_post(&apds_sem);
  }
}
#endif

#ifdef LIGHT_TASK
void * task_light(void * param)
{
  int32_t apds_handle;
  int32_t apds_irq_handle;
  struct sigaction apds_irq_action;
  int16_t ch0_data, ch1_data;
  float light;
  uint8_t op_flag = 0, retry = 0;
  int32_t initialized_flag = 1;
  /* Set up the light sensor */
  if(apds9301_init(&apds_handle)!=0)
  {
    // Error log
    initialized_flag = 0;
  }

#ifdef LIGHT_TASK_ALERT
  /* Set up the pin interrupt for handling light alert */
  memset(&apds_irq_action, 0, sizeof(apds_irq_action));
  apds_irq_action.sa_handler = apds_irq_handler;
  apds_irq_action.sa_flags = 0;
  if(sigaction(SIGIO, &apds_irq_action, NULL)!=0)
  {
    // Error log
    initialized_flag = 0;
  }
  if((apds_irq_handle=open("/dev/gpio_int", O_RDWR))<0)
  {
    // Error log
    initialized_flag = 0;
  }
  if(fcntl(apds_irq_handle, F_SETOWN, getpid())==-1)
  {
    // Error log
    initialized_flag = 0;
  }
  if(fcntl(apds_irq_handle, F_SETFL,
           fcntl(apds_irq_handle, F_GETFL) | FASYNC) == -1)
  {
    // Error log
    initialized_flag = 0;
  }
#endif

  /* Initialzie a semephore for ligth state change event */
  if(sem_init(&apds_sem, 0, 0)==-1)
  {
      //error log
      initialized_flag = 0;
  }

  while(initialized_flag)
  {
    /* Enqueue state changes to the msg queue */

    /* Wait for light state change event */
    sem_wait(&apds_sem);
    /* Update light state */
    while((retry<=RETRY_MAX)&&(op_flag==0))
    {
      if(apds_update_state(apds_handle, light_state)==-1)
      // if((i2c_read_word_mutex(apds_handle, CMD | WORD | DATA0_REG, &ch0_data)!=0) ||
      //    (i2c_read_word_mutex(apds_handle, CMD | WORD | DATA1_REG, &ch1_data)!=0))
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
      break;
    }

    op_flag = 0;

    /* Enqueue error message onto the queue */

    //light = apds_raw_to_lux(ch0_data, ch1_data);
    //printf("Light:%.2f\n", light);
  }

  /* THread clean up routine */
	close(apds_irq_handle);
  sem_destroy(&apds_sem);
  i2c_disconnect_mutex(apds_handle);
}

#endif

int32_t apds9301_init(int32_t *dev_fp)
{
  int32_t ret = 0;
  uint16_t ch0_data;
  /* Connect the apds9301 to the i2c interface */
  if(i2c_connect_mutex(dev_fp, APDS9301_ADDR)!=0)
    ret = -1;

  /* Power on the sensor */
  if(i2c_write_byte_mutex(*dev_fp, CMD | CONTROL_REG, POWER_ON)!=0)
    ret = -1;

  /* Configure the alert thresholds */
#ifdef LIGHT_TASK_ALERT
  /* Update the light state */
  if(i2c_read_word_mutex(*dev_fp, CMD | WORD | DATA0_REG, &ch0_data)!=0)
    ret = -1;
  light_state = ch0_data > DEFAULT_THRESH_VALUE ? LIGHT_STATE_DAY : LIGHT_STATE_NIGHT;
  /* Update the thresholds */
  if(apds_update_state(*dev_fp, light_state)==-1)
    ret = -1;
  /* Set up interrupt */
  if(i2c_write_byte_mutex(*dev_fp, CMD | INTERRUPT_REG,
                          (uint8_t)DEFAULT_INTERRUPT_SETTING))
    ret = -1;
#endif

  return ret;
}

static int32_t apds_update_state(int32_t dev_fp, uint32_t state)
{
  int32_t ret = 0;

  switch(state)
  {
    case LIGHT_STATE_DAY:
      light_state = LIGHT_STATE_NIGHT;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHLOW_REG,
                          (uint16_t)DEFAULT_THRESH_VALUE)!=0)
        ret = -1;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHHIGH_REG,
                          (uint16_t)THRESH_MAX)!=0)
        ret = -1;
      break;
    case LIGHT_STATE_NIGHT:
      light_state = LIGHT_STATE_DAY;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHLOW_REG,
                          (uint16_t)THRESH_MIN)!=0)
        ret = -1;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHHIGH_REG,
                          (uint16_t)DEFAULT_THRESH_VALUE)!=0)
        ret = -1;
      break;
    default:;
  }
  return ret;
}

static float apds_raw_to_lux(int16_t ch0, int16_t ch1)
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
