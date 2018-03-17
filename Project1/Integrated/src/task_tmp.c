#include "project.h"

pthread_mutex_t tmp_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tmp_cond = PTHREAD_COND_INITIALIZER;

static int32_t tmp1021_shutdown_enable(int32_t dev_fp);
static int32_t tmp1021_shutdown_disable(int32_t dev_fp);
static int32_t tmp1021_set_conversion_rate(int32_t dev_fp, uint16_t rate);
static int32_t tmp1021_get_conversion_rate(int32_t dev_fp, uint16_t * rate);
static int32_t tmp1021_set_em_mode(int32_t dev_fp, uint16_t em_mode);
static int32_t tmp1021_get_em_mode(int32_t dev_fp, uint16_t * em_mode);
static int32_t tmp1021_set_fault_queue(int32_t dev_fp, uint16_t faults_num);
static int32_t tmp1021_get_fault_queue(int32_t dev_fp, uint16_t * fault_num);
static int32_t tmp1021_set_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t value);
static int32_t tmp1021_get_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value);
static int32_t tmp1021_set_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t value);
static int32_t tmp1021_get_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value);
static float tmp_raw_to_temperature(int16_t raw, int32_t format);

#ifdef TEMP_TASK
void tmp_timer_handler(union sigval arg)
{
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

/* Missing:
 * - Hearbeat report
 * - Response to external request
 * - Temperature alert
 * - Enqueue temperature read to msg queue
 */
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
	  status = ERROR;
  }

  /* Periodic temperature read */
  while(status == SUCCESS)
  {
    /* Processing all msg in the queue */
    /* - Heartbeat request from main
       - External request */
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
      /* Break the loop and clean up */
      status = ERROR;
    }
    else
    {
      /* Convert digital data format to temperature */
      temperature = tmp_raw_to_temperature(tmp, CELSIUS_FORMAT);

      /* Enqueue the temperature onto the msg queue */
      tmp_msg = create_message_struct(TEMP_THREAD, LOGGERTHREAD, INFO, LOG_MSG);
      sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree C.",temperature);
      info.data = tmp_msg;
      info.thread_mutex_lock = log_queue_mutex;
      info.qName = LOGGER_QUEUE;
      status = msg_log(&info);
      printf("Temperature: %.2f\n", temperature);
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
					tmp_msg = create_message_struct(TEMP_THREAD, MAINTHREAD, HEARTBEAT,
							HEART_BEAT);
					info.data = tmp_msg;
					info.thread_mutex_lock = main_queue_mutex;
					info.qName = MAIN_QUEUE;
					status = msg_send(&info);
					break;
				case SHUT_DOWN:
          status = ERROR;
					break; //EXIT CODE
				case GET_TEMP_C:
					// Get the temperature value for external request
					tmp_msg = create_message_struct(TEMP_THREAD, SOCKETTHREAD, INFO,
							                            GET_TEMP_C);
          sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree C.",temperature);
					info.data = tmp_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					break;
        case GET_TEMP_K:
					// Get the temperature value for external request
					tmp_msg = create_message_struct(TEMP_THREAD, SOCKETTHREAD, INFO,
							                            GET_TEMP_K);
          sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree K.",temperature);
					info.data = tmp_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					break;
        case GET_TEMP_F:
					// Get the temperature value for external request
					tmp_msg = create_message_struct(TEMP_THREAD, SOCKETTHREAD, INFO,
							                            GET_TEMP_F);
          sprintf(tmp_msg.msg,"Temperature Value: %0.3f degree F.",temperature);
					info.data = tmp_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					break;
				default:;
			}
    }
#endif
  }

  /* Thread clean up routine */
  pthread_mutex_destroy(&tmp_mutex);
  pthread_cond_destroy(&tmp_cond);
  timer_delete(timer_id);
  i2c_disconnect_mutex(tmp_handle);
  pthread_mutex_destroy(&temp_queue_mutex);
  mq_unlink(TEMP_QUEUE);
  pthread_exit(NULL);
}
#endif

/*
 * Below are TMP1021 Utility APIs
 */

int32_t tmp1021_init(int32_t *dev_fp)
{
  /* Connect the tmp1021 to the i2c interface */
  if(i2c_connect_mutex(dev_fp, TMP1021_ADDR)!=0)
    return -1;
  /* Config the tmp1021 by default configuration */
  // if(i2c_write_word_mutex(*dev_fp, CONFIG_REG, CONFIG_DEFAULT)!=0)
  //   return -1;
#ifdef TEMP_TASK_ALERT
  // Setup thresholds
#endif
  return 0;
}

static int32_t tmp1021_shutdown_enable(int32_t dev_fp)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  if((reg & CONFIG_SD_MASK)!=CONFIG_SD_ENABLED)
  {
    reg = (reg & ~CONFIG_SD_MASK) | CONFIG_SD_ENABLED;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
      return -1;
  }
  return 0;
}

static int32_t tmp1021_shutdown_disable(int32_t dev_fp)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  if((reg & CONFIG_SD_MASK)!=CONFIG_SD_DISABLED)
  {
    reg = (reg & ~CONFIG_SD_MASK) | CONFIG_SD_DISABLED;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return -1;
    }
  return 0;
}

static int32_t tmp1021_set_conversion_rate(int32_t dev_fp, uint16_t rate)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  if((reg & CONFIG_CR_MASK)!=rate)
  {
    reg = (reg & ~CONFIG_CR_MASK) | rate;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return -1;
    }
  return 0;
}

static int32_t tmp1021_get_conversion_rate(int32_t dev_fp, uint16_t * rate)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  *rate = reg & CONFIG_CR_MASK;
  return 0;
}

static int32_t tmp1021_set_em_mode(int32_t dev_fp, uint16_t em_mode)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  if((reg & CONFIG_EM_MASK)!=em_mode)
  {
    reg = (reg & ~CONFIG_EM_MASK) | em_mode;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return -1;
    }
  return 0;
}

static int32_t tmp1021_get_em_mode(int32_t dev_fp, uint16_t * em_mode)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  *em_mode = reg & CONFIG_EM_MASK;
  return 0;
}

static int32_t tmp1021_set_fault_queue(int32_t dev_fp, uint16_t faults_num)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  if((reg & CONFIG_FQ_MASK)!=faults_num)
  {
    reg = (reg & ~CONFIG_FQ_MASK) | faults_num;
    if(i2c_write_word_mutex(dev_fp, CONFIG_REG, reg)==-1)
        return -1;
    }
  return 0;
}

static int32_t tmp1021_get_fault_queue(int32_t dev_fp, uint16_t * fault_num)
{
  uint16_t reg;
  if(i2c_read_word_mutex(dev_fp, CONFIG_REG, &reg)==-1)
    return -1;
  *fault_num = reg & CONFIG_FQ_MASK;
  return 0;
}

static int32_t tmp1021_set_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t value)
{
  value <<= 4;
  if(i2c_write_word_mutex(dev_fp, thresh_reg, value)==-1)
      return -1;
}

static int32_t tmp1021_get_threshold(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value)
{
  if(i2c_read_word_mutex(dev_fp, thresh_reg, value)==-1)
    return -1;
  *value >>= 4;
  return 0;
}

static int32_t tmp1021_set_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t value)
{
  value <<= 3;
  if(i2c_write_word_mutex(dev_fp, thresh_reg, value)==-1)
      return -1;
}

static int32_t tmp1021_get_threshold_ext(int32_t dev_fp, uint8_t thresh_reg, uint16_t * value)
{
  if(i2c_read_word_mutex(dev_fp, thresh_reg, value)==-1)
    return -1;
  *value >>= 3;
  return 0;
}

static float tmp_raw_to_temperature(int16_t raw, int32_t format)
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
