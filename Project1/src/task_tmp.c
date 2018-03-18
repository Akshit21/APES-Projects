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
* @file : task_tmp.c
* @brief : Temperature Thread Task
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/
#include "project.h"

pthread_mutex_t tmp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tmp_cond = PTHREAD_COND_INITIALIZER;

static Status_t tmp1021_shutdown_enable(int32_t dev_fp);
static Status_t tmp1021_shutdown_disable(int32_t dev_fp);
static Status_t tmp1021_set_conversion_rate(int32_t dev_fp, uint16_t rate);
static Status_t tmp1021_get_conversion_rate(int32_t dev_fp, uint16_t * rate);
static Status_t tmp1021_set_em_mode(int32_t dev_fp, uint16_t em_mode);
static Status_t tmp1021_get_em_mode(int32_t dev_fp, uint16_t * em_mode);
static Status_t tmp1021_set_fault_queue(int32_t dev_fp, uint16_t faults_num);
static Status_t tmp1021_get_fault_queue(int32_t dev_fp, uint16_t * fault_num);
static Status_t tmp1021_set_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t value);
static Status_t tmp1021_get_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value);
static Status_t tmp1021_set_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t value);
static Status_t tmp1021_get_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value);
static float tmp_raw_to_temperature(int16_t raw);

#ifdef TEMP_TASK
/* Timer rountine */
void tmp_timer_handler(union sigval arg)
{
	DEBUG("[DEBUG] TMP timer timeout.\n");
  pthread_mutex_lock(&tmp_mutex);
	pthread_cond_signal(&tmp_cond);
  pthread_mutex_unlock(&tmp_mutex);
}

#ifdef TEMP_TASK_ALERT
void tmp_irq_handler(int signo)
{
  if(signo == SIGIO)
  {

  }
}
#endif

void * task_tmp(void * param)
{
  int32_t tmp_handle = *(int32_t *)param;
  int32_t tmp_irq_handle;
  struct sigaction tmp_irq_action;
  uint16_t tmp;
  timer_t timer_id;
  float temperature;
  uint8_t op_flag = 0, retry = 0;
  Status_t status = SUCCESS;
  Message_t tmp_msg;
  ThreadInfo_t info;

#ifdef TEMP_TASK_ALERT
  /* Set up the pin interrupt for handling light alert */
  memset(&tmp_irq_action, 0, sizeof(tmp_irq_action));
  tmp_irq_action.sa_handler = tmp_irq_handler;
  tmp_irq_action.sa_flags = 0;
  if(sigaction(SIGIO, &tmp_irq_action, NULL)!=0)
  {
    // Error log
    status = ERROR;
  }
  if((tmp_irq_handle=open("/dev/gpio_tmp_int", O_RDWR))<0)
  {
    // Error log
    status = ERROR;
  }
  if(fcntl(tmp_irq_handle, F_SETOWN, getpid())==-1)
  {
    // Error log
    status = ERROR;
  }
  if(fcntl(tmp_irq_handle, F_SETFL,
     fcntl(tmp_irq_handle, F_GETFL) | FASYNC) == -1)
  {
    // Error log
    status = ERROR;
  }
#endif
  /* Set up a periodic timer */
  if((timer_setup(&timer_id, TMP_PERIOD, tmp_timer_handler))==-1)
  {
  	// error;
		DEBUG("[DEBUG] Failed to set up tmp timer.\n");
	  status = ERROR;
  }

  /* Periodic temperature read */
  while(status == SUCCESS)
  {

    pthread_mutex_lock(&tmp_mutex);
	  pthread_cond_wait(&tmp_cond, &tmp_mutex);
    pthread_mutex_unlock(&tmp_mutex);

		/* Read temperature raw data from the sensor */
    while((retry<=RETRY_MAX) && (op_flag==0))
    {
      if(i2c_read_word_mutex(tmp_handle, TEMP_REG, &tmp)==-1)
      {
        retry ++;
      }
      else
      {
        retry = 0;
        op_flag = 1;
      }
    }
    if(op_flag==0)
    {
      // Error
      DEBUG("[DEBUG] Failed to read from sensor consecutively. Exit.\n");
			tmp_msg = create_message_struct(TEMP_THREAD, LOGGERTHREAD, ERROR, LOG_MSG);
			sprintf(tmp_msg.msg,"Tempreature sensor failed.");
			info.data = tmp_msg;
			info.thread_mutex_lock = log_queue_mutex;
			info.qName = LOGGER_QUEUE;
			status = msg_log(&info);
      status = ERROR;
			blinkLED();
    }
    else
    {
      /* Convert digital data format to temperature */
      temperature = tmp_raw_to_temperature(tmp);
			DEBUG("Temperature: %.2f\n", temperature);
      /* Enqueue the temperature onto the msg queue */
#ifdef TEMP_TASK_MESSAGING
      tmp_msg = create_message_struct(TEMP_THREAD, LOGGERTHREAD, INFO, LOG_MSG);
      sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree C.",temperature);
      info.data = tmp_msg;
      info.thread_mutex_lock = log_queue_mutex;
      info.qName = LOGGER_QUEUE;
      status = msg_log(&info);
#endif
    }

#ifdef TEMP_TASK_MESSAGING
    /* Process message queue */
    while(temp_queue_flag)
    {
		  memset(&info.data.msg, 0, sizeof(info.data.msg));
      temp_queue_flag --;
      info.thread_mutex_lock = temp_queue_mutex;
			info.qName = TEMP_QUEUE;
			msg_receive(&info);
			tmp_msg = info.data;
			switch(tmp_msg.requestId)
			{
				case HEART_BEAT:
					DEBUG("[DEBUG] TMP task received HEARTBEAT request.\n");
					tmp_msg = create_message_struct(TEMP_THREAD, MAINTHREAD, HEARTBEAT, HEART_BEAT);
					info.data = tmp_msg;
					info.thread_mutex_lock = main_queue_mutex;
					info.qName = MAIN_QUEUE;
					status = msg_send(&info);
					DEBUG("[DEBUG] TMP task responded to HEARTBEAT request.\n");
					break;
				case SHUT_DOWN:
					DEBUG("[DEBUG] TMP task received SHUTDOWN request.\n");
          status = ERROR;
					break;
				case GET_TEMP_C:
					DEBUG("[DEBUG] TMP task received GET_TEMP_C request.\n");
					// Get the temperature value for external request
					tmp_msg = create_message_struct(TEMP_THREAD, SOCKETTHREAD, INFO, GET_TEMP_C);
					sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree C.",temperature);
					info.data = tmp_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					DEBUG("[DEBUG] TMP task responded to GET_TEMP_C request.\n");
					break;
        case GET_TEMP_K:
					DEBUG("[DEBUG] TMP task received GET_TEMP_K request.\n");
					tmp_msg = create_message_struct(TEMP_THREAD, SOCKETTHREAD, INFO, GET_TEMP_K);
          sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree K.",temperature + 273.15);
					info.data = tmp_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					DEBUG("[DEBUG] TMP task responded to GET_TEMP_K request.\n");
					break;
        case GET_TEMP_F:
					DEBUG("[DEBUG] TMP task received GET_TEMP_F request.\n");
					tmp_msg = create_message_struct(TEMP_THREAD, SOCKETTHREAD, INFO, GET_TEMP_F);
          sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree F.",temperature*1.8 + 32);
					info.data = tmp_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					DEBUG("[DEBUG] TMP task responded to GET_TEMP_F request.\n");
					break;
				default:;
			}
    }
#endif
  }

  /* Thread clean up routine */
	DEBUG("[DEBUG] TMP task start clean up routine and exit.\n");
  pthread_mutex_destroy(&tmp_mutex);
  pthread_cond_destroy(&tmp_cond);
  timer_delete(timer_id);
  i2c_disconnect_mutex(tmp_handle);
  pthread_mutex_destroy(&temp_queue_mutex);
  mq_unlink(TEMP_QUEUE);
	/* Blink LED */
  blinkLED();
  pthread_exit(NULL);
}
#endif

/*
 * Below are TMP1021 Utility APIs
 */

 /**
 * @brief Function to initialize the tmp1021 sensor
 *
 * @param a handle to store the file descriptor
 *
 * @return Status SUCCES/ERROR
 */
Status_t tmp1021_init(int32_t *dev_fp)
{
  /* Connect the tmp1021 to the i2c interface */
  if(i2c_connect_mutex(dev_fp, TMP1021_ADDR)!=0)
    return ERROR;
  /* Config the tmp1021 by default configuration */
  // if(i2c_write_word_mutex(*dev_fp, CONFIG_REG, CONFIG_DEFAULT)!=0)
  //   return -1;
#ifdef TEMP_TASK_ALERT
  // Setup thresholds
#endif
  return SUCCESS;
}

/**
* @brief Function to enable the shutdown mode of the tmp1021 sensor
*
* @param a handle to to the device file descriptor
*
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_shutdown_enable(int32_t dev_fp)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  if((reg & CONFIG_SD_MASK)!=CONFIG_SD_ENABLED)
  {
    reg = (reg & ~CONFIG_SD_MASK) | CONFIG_SD_ENABLED;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
    	return ERROR;
  }
  return SUCCESS;
}

/**
* @brief Function to disable the shutdown mode of the tmp1021 sensor
*
* @param a handle to to the device file descriptor
*
* @return Status SUCCES/ERROR
*/

static Status_t tmp1021_shutdown_disable(int32_t dev_fp)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  if((reg & CONFIG_SD_MASK)!=CONFIG_SD_DISABLED)
  {
    reg = (reg & ~CONFIG_SD_MASK) | CONFIG_SD_DISABLED;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return ERROR;
    }
  return SUCCESS;
}

/**
* @brief Function to set the conversion rate of the tmp1021 sensor
*
* @param dev_fp - a handle to to the device file descriptor
*        rate - the rate to set
*
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_set_conversion_rate(int32_t dev_fp, uint16_t rate)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  if((reg & CONFIG_CR_MASK)!=rate)
  {
    reg = (reg & ~CONFIG_CR_MASK) | rate;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return ERROR;
    }
  return SUCCESS;
}

/**
* @brief Function to get the shutdown mode of the tmp1021 sensor
*
* @param dev-fp - a handle to to the device file descriptor
*        rate - a pointer to a variable to store the rate read
*
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_get_conversion_rate(int32_t dev_fp, uint16_t * rate)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  *rate = reg & CONFIG_CR_MASK;
  return SUCCESS;
}

/**
* @brief Function to set the em mode of the tmp1021 sensor
*
* @param dev_fp - a handle to to the device file descriptor
*        em_mode - the mode to set, only two modes are valid:
									 CONFIG_EM_NORMAL_MODE / CONFIG_EM_EXTENDED_MODE
*
* @return Status SUCCES/ERROR
*/

static Status_t tmp1021_set_em_mode(int32_t dev_fp, uint16_t em_mode)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  if((reg & CONFIG_EM_MASK)!=em_mode)
  {
    reg = (reg & ~CONFIG_EM_MASK) | em_mode;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return ERROR;
    }
  return SUCCESS;
}

/**
* @brief Function to get the em mode of the tmp1021 sensor
*
* @param dev_fp - a handle to to the device file descriptor
*        em_mode - a pointer to a variable to store the mode read
*
* @return Status SUCCES/ERROR
*/

static Status_t tmp1021_get_em_mode(int32_t dev_fp, uint16_t * em_mode)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  *em_mode = reg & CONFIG_EM_MASK;
  return SUCCESS;
}

/**
* @brief Function to set the fault queue of the tmp1021 sensor
*
* @param dev_fp - a handle to to the device file descriptor
*		     faults_num - the number of consecutive faults to set, only below are valid:
*					CONFIG_FQ_CONSECUTIVE_FAULTS_1
*					CONFIG_FQ_CONSECUTIVE_FAULTS_2
*					CONFIG_FQ_CONSECUTIVE_FAULTS_3
*					CONFIG_FQ_CONSECUTIVE_FAULTS_4
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_set_fault_queue(int32_t dev_fp, uint16_t faults_num)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  if((reg & CONFIG_FQ_MASK)!=faults_num)
  {
    reg = (reg & ~CONFIG_FQ_MASK) | faults_num;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return ERROR;
  }
  return SUCCESS;
}

/**
* @brief Function to get the fault queue setting of the tmp1021 sensor
*
* @param dev_fp - a handle to to the device file descriptor
*				 fault_num - a pointer to the variable to store the value read
*
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_get_fault_queue(int32_t dev_fp, uint16_t * fault_num)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return ERROR;
  *fault_num = reg & CONFIG_FQ_MASK;
  return SUCCESS;
}

/**
* @brief Function to set the int thresholds of the tmp1021 sensor
*
* @param dev_fp - a handle to to the device file descriptor
*        thresh_reg - the address of the threshold register
*				 value - threshold value to set
*
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_set_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t value)
{
  value <<= 4;
  if(i2c_write_word_mutex(dev_fp, thresh_reg, value)==-1)
      return ERROR;
	return SUCCESS;
}

/**
* @brief Function to get the int thresholds of the tmp1021 sensor
*
* @param dev_fp - a handle to to the device file descriptor
*        thresh_reg - the address of the threshold register
*				 value - a pointer to the variable to store the threshold value read
*
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_get_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value)
{
  if(i2c_read_word_mutex(dev_fp, thresh_reg, value)==-1)
    return ERROR;
  *value >>= 4;
  return SUCCESS;
}

/**
* @brief Function to set the int thresholds of the tmp1021 sensor in extended mode
*
* @param dev_fp - a handle to to the device file descriptor
*        thresh_reg - the address of the threshold register
*				 value - threshold value to set
*
* @return Status SUCCES/ERROR
*/
static Status_t tmp1021_set_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t value)
{
  value <<= 3;
  if(i2c_write_word_mutex(dev_fp, thresh_reg, value)==-1)
      return ERROR;
	return SUCCESS;
}

/**
* @brief Function to get the int thresholds of the tmp1021 sensor in extended mode
*
* @param dev_fp - a handle to to the device file descriptor
*        thresh_reg - the address of the threshold register
*				 value - a pointer to the variable to store the threshold value read
*
* @return Status SUCCES/ERROR
*/

static Status_t tmp1021_get_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value)
{
  if(i2c_read_word_mutex(dev_fp, thresh_reg, value)==-1)
    return ERROR;
  *value >>= 3;
  return SUCCESS;
}

/**
* @brief Function to convert raw digital data to temperature value in degree C
*
* @param raw - raw value
*
* @return Status SUCCES/ERROR
*/
static float tmp_raw_to_temperature(int16_t raw)
{
  float temperature;
  raw = (int16_t)((raw&0xf000)>>12) | ((raw&0x00ff)<<4);
  /* If a negative value */
  if(raw & 0x0800)
  {
	  raw ^= 0xffff;
	  raw += 1;
	  temperature = -(raw*0.0625);
  }
	/* If the value is positive */
  else
  	temperature  = raw * 0.0625;

  return temperature;
}
