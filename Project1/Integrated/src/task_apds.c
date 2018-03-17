
#include "project.h"

sem_t apds_sem;

char *light_state_s[] = {"DAY", "NIGHT"};
int32_t light_state;

static int32_t apds_update_state(int32_t dev_fp, uint32_t state);
static float apds_raw_to_lux(int16_t ch0, int16_t ch1);

#ifdef LIGHT_TASK_ALERT
void apds_irq_handler(int signo)
{
  if(signo == SIGIO)
  {
    printf("light alert!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\n");
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
  Status_t status = SUCCESS;
  const struct timespec sem_timeout = {.tv_sec = 1, .tv_nsec=0};
  Message_t apds_msg;
  ThreadInfo_t info;

  /* Set up the light sensor */
  if(apds9301_init(&apds_handle)!=0)
  {
    // Error log
    status = ERROR;
  }

#ifdef LIGHT_TASK_ALERT
  /* Set up the pin interrupt for handling light alert */
  memset(&apds_irq_action, 0, sizeof(apds_irq_action));
  apds_irq_action.sa_handler = apds_irq_handler;
  apds_irq_action.sa_flags = 0;
  if(sigaction(SIGIO, &apds_irq_action, NULL)!=0)
  {
    // Error log
    status = ERROR;
  }
  if((apds_irq_handle=open("/dev/gpio_int", O_RDWR))<0)
  {
    // Error log
    status = ERROR;
  }
  if(fcntl(apds_irq_handle, F_SETOWN, getpid())==-1)
  {
    // Error log
    status = ERROR;
  }
  if(fcntl(apds_irq_handle, F_SETFL,
           fcntl(apds_irq_handle, F_GETFL) | FASYNC) == -1)
  {
    // Error log
    status = ERROR;
  }
#endif

  /* Initialzie a semephore for ligth state change event */
  if(sem_init(&apds_sem, 0, 0)==-1)
  {
      //error log
      status = ERROR;
  }
  printf("setup light irq\n");

  while(status == SUCCESS)
  {
	  sleep(1);
	  printf("light thread is here\n");
    /* Enqueue state changes to the msg queue */

    /* Wait for light state change event */
    if(sem_timedwait(&apds_sem, &sem_timeout)!=-1)
    {
	i2c_write_byte_mutex(apds_handle, CMD|CLEAR|CONTROL_REG, 0x03);
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
        status = ERROR;
      }
      else
      {
	printf("update\n");
        op_flag = 0;
#ifdef LIGHT_TASK_MESSAGING
        apds_msg = create_message_struct(LIGHT_THREAD, LOGGERTHREAD, INFO, LOG_MSG);
        sprintf(apds_msg.msg,"Light state changed to: %s",light_state_s[light_state]);
        info.data = apds_msg;
        info.thread_mutex_lock = log_queue_mutex;
        info.qName = LOGGER_QUEUE;
        status = msg_log(&info);
#endif
      }
    }
    /* Enqueue error message onto the queue */

    //light = apds_raw_to_lux(ch0_data, ch1_data);
    //printf("Light:%.2f\n", light);
#ifdef LIGHT_TASK_MESSAGING
    /* Process message queue */;
		while (light_queue_flag)
		{
			printf("light:::::::::::::::::receiving....\n");
			light_queue_flag--;

		  memset(&info.data.msg, 0, sizeof(info.data.msg));
			info.thread_mutex_lock = light_queue_mutex;
			info.qName = LIGHT_QUEUE;
      if((status = msg_receive(&info))==SUCCESS)
      {
	      printf("@#$%^&*()_------------------------------------------------\n");
			  apds_msg = info.data;
			  switch(apds_msg.requestId)
			  {
				  case HEART_BEAT:
					  printf("light:hb requested....%d****\n", LIGHT_THREAD);
					  apds_msg = create_message_struct(LIGHT_THREAD, MAINTHREAD, HEARTBEAT,
							                             HEART_BEAT);
					  info.data = apds_msg;
					  info.thread_mutex_lock = main_queue_mutex;
					  info.qName = MAIN_QUEUE;
					  status = msg_send(&info);
					  break;
				  case SHUT_DOWN:
            status = ERROR;
					  break; //EXIT CODE
				  case GET_LIGHT:
					  // Get the light value for external request
					  apds_msg = create_message_struct(LIGHT_THREAD, SOCKETTHREAD, INFO,
                                             GET_LIGHT);
            // populate lux value
					  info.data = apds_msg;
					  info.thread_mutex_lock = socket_queue_mutex;
					  info.qName = SOCKET_QUEUE;
					  msg_send(&info);
					  break;
				  case GET_LIGHT_STATE:
					// Get the light state for external request
					  apds_msg = create_message_struct(LIGHT_THREAD, SOCKETTHREAD, INFO,
						                                 GET_LIGHT);
            // Populate light state
            info.data = apds_msg;
					  info.thread_mutex_lock = socket_queue_mutex;
					  info.qName = SOCKET_QUEUE;
					  msg_send(&info);
					  break;
				  default:;
			  }
      }
		}
#endif
  }
	printf("light exit!!!!!!!!!!!!!!!!!!!!\n");
  /* THread clean up routine */
	close(apds_irq_handle);
  sem_destroy(&apds_sem);
  i2c_disconnect_mutex(apds_handle);
  pthread_mutex_destroy(&light_queue_mutex);
  mq_unlink(LIGHT_QUEUE);
  pthread_exit(NULL);
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
  light_state = ch0_data < DEFAULT_THRESH_VALUE ? LIGHT_STATE_DAY : LIGHT_STATE_NIGHT;
  /* Update the thresholds */
  if(apds_update_state(*dev_fp, light_state)==-1)
    ret = -1;
  /* Set up interrupt */
  if(i2c_write_byte_mutex(*dev_fp, CMD | INTERRUPT_REG,
                          (uint8_t)DEFAULT_INTERRUPT_SETTING)==-1)
    ret = -1;
#endif
  if(i2c_write_byte_mutex(*dev_fp, CMD|CLEAR|CONTROL_REG, 0x03)==-1)
	  ret =-1;
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
