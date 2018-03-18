/**************************************************************************************************
* Copyright (C) 2017 by Akshit Shah, Shuting Guo
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Akshit Shah, Shuting Guo, Prof. Alex Fosdick and the University of Colorado are
* not liable for any misuse of this material.
***************************************************************************************************/
/***************************************************************************************************
* @author : Akshit Shah, Shuting Guo
* @date : 03/1/2018
*
* @file : task_apds.c
* @brief : Light Thread Task
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/
#include "project.h"

sem_t apds_sem;

char *light_state_s[] = {"DAY", "NIGHT"};
int32_t light_state;

static Status_t apds_update_state(int32_t dev_fp);

static float apds_raw_to_lux(int16_t ch0, int16_t ch1);

#ifdef LIGHT_TASK_ALERT
/* Light interrupt IRQ */
void apds_irq_handler(int signo)
{
  if(signo == SIGIO)
  {
    DEBUG("[DEBUG] Light state change interrupted.\n");
  	sem_post(&apds_sem);
  }
}
#endif

#ifdef LIGHT_TASK
void * task_light(void * param)
{
  int32_t apds_handle = *(int32_t*)param;
  int32_t apds_irq_handle;
  struct sigaction apds_irq_action;
  int16_t ch0_data, ch1_data;
  float light;
  uint8_t op_flag = 0, retry = 0;
  Status_t status = SUCCESS;
  const struct timespec sem_timeout = {.tv_sec = 1, .tv_nsec=0};
  Message_t apds_msg;
  ThreadInfo_t info;

#ifdef LIGHT_TASK_ALERT
  /* Set up the pin interrupt for handling light alert */
  memset(&apds_irq_action, 0, sizeof(apds_irq_action));
  apds_irq_action.sa_handler = apds_irq_handler;
  apds_irq_action.sa_flags = 0;
  if(sigaction(SIGIO, &apds_irq_action, NULL)!=0)
  {
    DEBUG("[DEBUG] sigaction failed.\n");
    status = ERROR;
  }
  if((apds_irq_handle=open("/dev/gpio_int", O_RDWR))<0)
  {
    status = ERROR;
  }
  if(fcntl(apds_irq_handle, F_SETOWN, getpid())==-1)
  {
    status = ERROR;
  }
  if(fcntl(apds_irq_handle, F_SETFL,
           fcntl(apds_irq_handle, F_GETFL) | FASYNC) == -1)
  {
    status = ERROR;
  }
#endif

  /* Initialzie a semephore for ligth state change event */
  if(sem_init(&apds_sem, 0, 0)==-1)
  {
      status = ERROR;
  };

  while(status == SUCCESS)
  {
    sleep(1);

    /* Wait for light state change event */
    if(sem_timedwait(&apds_sem, &sem_timeout)!=-1)
    {
	     i2c_write_byte_mutex(apds_handle, CMD|CLEAR|CONTROL_REG, 0x03);
       /* Update light state */
	     apds_update_state(apds_handle);
       while((retry<=RETRY_MAX)&&(op_flag==0))
       {
         /* Read value from both channel */
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
        DEBUG("[DEBUG] Failed to change light stats.\n");
        apds_msg = create_message_struct(LIGHT_THREAD, LOGGERTHREAD, ERROR, LOG_MSG);
        sprintf(apds_msg.msg,"Light sensor failed");
        info.data = apds_msg;
        info.thread_mutex_lock = log_queue_mutex;
        info.qName = LOGGER_QUEUE;
        status = msg_log(&info);
        status = ERROR;
        blinkLED();
       }
       else
       {
	        DEBUG("[DEBUG] LIGHT task updated light state to: %s\n", light_state_s[light_state]);
          op_flag = 0;
          /* Calculate lux */
	        light = apds_raw_to_lux(ch0_data, ch1_data);
	        DEBUG("[DEBUG] LIGHT task get light: %.3f lux\n", light);
#ifdef LIGHT_TASK_MESSAGING
          /* Enqueue the light state change info to log queue */
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
#ifdef LIGHT_TASK_MESSAGING
    /* Process message queue */;
    while (light_queue_flag)
		{
      light_queue_flag--;
		  memset(&info.data.msg, 0, sizeof(info.data.msg));
			info.thread_mutex_lock = light_queue_mutex;
			info.qName = LIGHT_QUEUE;
      if((status = msg_receive(&info))==SUCCESS)
      {
			  apds_msg = info.data;
			  switch(apds_msg.requestId)
			  {
				  case HEART_BEAT:
					  DEBUG("[DEBUG] LIGHT task received HEARTBEAT request.\n");
					  apds_msg = create_message_struct(LIGHT_THREAD, MAINTHREAD, HEARTBEAT, HEART_BEAT);
					  info.data = apds_msg;
					  info.thread_mutex_lock = main_queue_mutex;
					  info.qName = MAIN_QUEUE;
					  status = msg_send(&info);
            DEBUG("[DEBUG] LIGHT task responded to HEARTBEAT request.\n");
					  break;
				  case SHUT_DOWN:
            DEBUG("[DEBUG] LIGHT task received SHUTDOWN request.\n");
            status = ERROR;
					  break; //EXIT CODE
				  case GET_LIGHT:
					  DEBUG("[DEBUG] LIGHT task received GET_LIGHT request.\n");
					  apds_msg = create_message_struct(LIGHT_THREAD, SOCKETTHREAD, INFO, GET_LIGHT);
            // populate lux value
            sprintf(apds_msg.msg,"Light Value: %0.3f.",light);
					  info.data = apds_msg;
					  info.thread_mutex_lock = socket_queue_mutex;
					  info.qName = SOCKET_QUEUE;
					  msg_send(&info);
            DEBUG("[DEBUG] LIGHT task responded to GET_LIGHT request.\n");
					  break;
				  case GET_LIGHT_STATE:
					  DEBUG("[DEBUG] LIGHT task received GET_LIGHT_STATE request.\n");
					  apds_msg = create_message_struct(LIGHT_THREAD, SOCKETTHREAD, INFO, GET_LIGHT_STATE);
            // Populate light state
            sprintf(apds_msg.msg,"Light STATE: %s.",light_state_s[light_state]);
            info.data = apds_msg;
					  info.thread_mutex_lock = socket_queue_mutex;
					  info.qName = SOCKET_QUEUE;
					  msg_send(&info);
            DEBUG("[DEBUG] LIGHT task responded to GET_LIGHT_STATE request.\n");
					  break;
				  default:;
			  }
      }
		}
#endif
  }
	DEBUG("[DEBUG] LIGHT task exits.\n");
  /* THread clean up routine */
	close(apds_irq_handle);
  sem_destroy(&apds_sem);
  i2c_disconnect_mutex(apds_handle);
  pthread_mutex_destroy(&light_queue_mutex);
  mq_unlink(LIGHT_QUEUE);
  /* Blink LED */
  blinkLED();
  pthread_exit(NULL);
}

#endif

/**
* @brief Function to initialize the apds9301 sensor
*
* @param a handle to store the file descriptor
*
* @return Status SUCCES/ERROR
*/
Status_t apds9301_init(int32_t *dev_fp)
{
  uint16_t ch0_data;
  /* Connect the apds9301 to the i2c interface */
  if(i2c_connect_mutex(dev_fp, APDS9301_ADDR)!=0)
    return ERROR;

  /* Power on the sensor */
  if(i2c_write_byte_mutex(*dev_fp, CMD | CONTROL_REG, POWER_ON)!=0)
    return ERROR;

  /* Configure the alert thresholds */
#ifdef LIGHT_TASK_ALERT
  /* Update the light state */
  if(i2c_read_word_mutex(*dev_fp, CMD | WORD | DATA0_REG, &ch0_data)!=0)
    return ERROR;
  light_state = ch0_data < DEFAULT_THRESH_VALUE ? LIGHT_STATE_DAY : LIGHT_STATE_NIGHT;
  /* Update the thresholds */
  if(apds_update_state(*dev_fp)==ERROR)
    return ERROR;
  /* Set up interrupt */
  if(i2c_write_byte_mutex(*dev_fp, CMD | INTERRUPT_REG,
                          (uint8_t)DEFAULT_INTERRUPT_SETTING)==-1)
    return ERROR;
#endif
  if(i2c_write_byte_mutex(*dev_fp, CMD|CLEAR|CONTROL_REG, 0x03)==-1)
	  return ERROR;
  return SUCCESS;
}

/**
* @brief Function to update the light state
*
* @param a handle to store the file descriptor
*
* @return Status SUCCES/ERROR
*/
static Status_t apds_update_state(int32_t dev_fp)
{
  switch(light_state)
  {
    case LIGHT_STATE_DAY:
      light_state = LIGHT_STATE_NIGHT;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHLOW_REG,
                          (uint16_t)THRESH_MIN)!=0)
        return ERROR;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHHIGH_REG,
                          (uint16_t)DEFAULT_THRESH_VALUE)!=0)
        return ERROR;
      break;
    case LIGHT_STATE_NIGHT:
      light_state = LIGHT_STATE_DAY;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHLOW_REG,
                          (uint16_t)DEFAULT_THRESH_VALUE)!=0)
        return ERROR;
      if(i2c_write_word_mutex(dev_fp, CMD | WORD | THRESHHIGH_REG,
                          (uint16_t)THRESH_MAX)!=0)
        return ERROR;
      break;
    default:;
  }
  return SUCCESS;
}

/**
* @brief Function to convert raw sensor data to lux value
*
* @param ch0 - value from channel 0
*        ch1 - value from channel 1
*
* @return value in lux
*/
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
