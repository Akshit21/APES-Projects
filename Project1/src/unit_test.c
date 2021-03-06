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
* @date : 03/15/2018
*
* @file : unit_test.c
* @brief : Unit testing important modules
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "project.h"

void test_apds9301(void** state)
{
  uint8_t read_byte;
  uint16_t read_word;
  int32_t apds_handle;
  /* Test connecting to device */
  assert_int_equal(i2c_connect_mutex(&apds_handle, APDS9301_ADDR), 0);

  /* Test power-on */
  assert_int_equal(i2c_write_byte_mutex(apds_handle, CMD | CONTROL_REG,
                                        (uint8_t)POWER_ON), 0);
  assert_int_equal(i2c_read_byte_mutex(apds_handle, CMD | CONTROL_REG,
                                       &read_byte), 0);
  read_byte &= 0x03;
  assert_true(read_byte == (uint8_t)POWER_ON);

  /* Test setting up thresholds and interrupt */
  assert_int_equal(i2c_write_word_mutex(apds_handle, CMD|WORD|THRESHLOW_REG,
                                        (uint16_t)DEFAULT_THRESH_VALUE), 0);
  assert_int_equal(i2c_read_word_mutex(apds_handle, CMD|WORD|THRESHLOW_REG,
                                       &read_word), 0);
  assert_true(read_word == (int16_t)DEFAULT_THRESH_VALUE);
  assert_int_equal(i2c_write_byte_mutex(apds_handle, CMD | INTERRUPT_REG,
                                        (uint8_t)DEFAULT_INTERRUPT_SETTING), 0);
  assert_int_equal(i2c_read_byte_mutex(apds_handle, CMD | INTERRUPT_REG,
                                       &read_byte), 0);
  assert_true(read_byte == (uint8_t)DEFAULT_INTERRUPT_SETTING);

  /* Power off the device */
  assert_int_equal(i2c_write_byte_mutex(apds_handle, CMD|CONTROL_REG,
                                        POWER_OFF), 0);
  assert_int_equal(i2c_read_byte_mutex(apds_handle, CMD|CONTROL_REG,
                                      &read_byte), 0);
  assert_true(read_byte == 0x00);
  /* Disconnect */
  i2c_disconnect_mutex(apds_handle);
}

void test_tmp1021(void** state)
{
  uint8_t read_byte;
  int16_t read_word;
  int32_t tmp_handle;
  /* Test connecting to device */
  assert_int_equal(i2c_connect_mutex(&tmp_handle, TMP1021_ADDR), 0);
  /* Test configuring sensor register */
  //tmp1021_shutdown_enable(tmp_handle);
  //i2c_read_byte_mutex(tmp_handle, CONFIG_REG, &read_byte);
  //assert_true(read_byte & CONFIG_SD_MASK == CONFIG_SD_ENABLED);
  /* Disconnect */
  i2c_disconnect_mutex(tmp_handle);
}

void test_initialise_queue(void **state)
{
	//initialise queue attributes
	Status_t status = SUCCESS;

  struct mq_attr attr;
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = sizeof(Message_t);
	attr.mq_flags = 0;

	int32_t log_queue =  mq_open(LOGGER_QUEUE,O_RDWR | O_CREAT,0666,&attr);
	int32_t temp_queue = mq_open(TEMP_QUEUE,O_RDWR | O_CREAT,0666,&attr);

	if(log_queue == -1 || temp_queue == -1)
	{
		status = ERROR;
	}

  assert_int_equal(status,SUCCESS);
}

void test_queue_send_receive(void **state)
{
	//initialise queue attributes
	Status_t status = SUCCESS;

	Message_t msg = create_message_struct(TEMP_THREAD, LOGGERTHREAD, INFO, LOG_MSG);
  sprintf(msg.msg,"DUMMY TEMP DATA");
  ThreadInfo_t info;

  info.data = msg;
  info.thread_mutex_lock = log_queue_mutex;
  info.qName = LOGGER_QUEUE;
  msg_send(&info);

  memset(&info, 0, sizeof(info));
  info.thread_mutex_lock = log_queue_mutex;
  info.qName = LOGGER_QUEUE;
  if(log_queue_flag)
  {
    status = msg_receive(&info);
  }

  assert_int_equal(status,SUCCESS);
}

void test_logger(void **state)
{
  Status_t status;
  Message_t msg = create_message_struct(TEMP_THREAD, LOGGERTHREAD, INFO, LOG_MSG);
  sprintf(msg.msg, "DUMMY TEMP VALUE: 20 deg C");
  ThreadInfo_t info;

  info.data = msg;
  info.thread_mutex_lock = log_queue_mutex;
  info.qName = LOGGER_QUEUE;
  status = msg_send(&info);
  assert_int_equal(status, SUCCESS);

  if(log_queue_flag)
  {
    status = msg_receive(&info);
    assert_int_equal(status,SUCCESS);
    FILE *pfile;
    const char *fileName = "logFile.txt";
    /* Open the file to log */
    if((pfile=fopen(fileName,"w")) == NULL)
  	{
      status = ERROR;
  	}
    fclose(pfile);

    status = log_data(&pfile, &info.data, fileName);
    assert_int_equal(status, SUCCESS);
  }
}

int main(int argc, char ** argv)
{
  const struct CMUnitTest tests[] =
  {
    cmocka_unit_test(test_apds9301),
    cmocka_unit_test(test_tmp1021),
    cmocka_unit_test(test_initialise_queue),
    cmocka_unit_test(test_queue_send_receive),
    cmocka_unit_test(test_logger),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
