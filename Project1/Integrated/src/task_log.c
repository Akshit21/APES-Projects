#include "project.h"

Status_t log_data(FILE **pfile, Message_t *message, const char *fileName);

void *task_log(void *param)
{
  Message_t log_msg;
  ThreadInfo_t info;
  Status_t status = SUCCESS;
	FILE *pfile;
  const char *fileName = "logFile.txt";

  /* Open the file to log */
  if((pfile=fopen(fileName,"w")) == NULL)
	{
    status = ERROR;
	}

	while (status == SUCCESS)
	{
#ifdef LOGGER_TASK_MESSAGING
		while (log_queue_flag)
		{
			log_queue_flag--;
      memset(info.data.msg, 0, sizeof(info.data.msg));
			info.thread_mutex_lock = log_queue_mutex;
			info.qName = LOGGER_QUEUE;
			if((status = msg_receive(&info))==SUCCESS)
			{
        log_msg = info.data;
			  switch(log_msg.requestId)
			  {
				  case LOG_MSG:
					  // printf ("Source ID: %d \n", log_msg.sourceId);
					  // printf ("Timestamp: %s", ctime(&log_msg.timeStamp));
					  // printf ("Log Level: %s \n", levels[log_msg.type]);
					  // printf ("Message Data: %s \n", log_msg.msg);
            DEBUG("[DEBUF] LOGGER task received LOG_MSG request.\n");
					  status = log_data(&pfile, &log_msg, fileName);
            DEBUG("[DEBUG] LOGGER task logged data.\n");
					  break;
				  case HEART_BEAT:
					  DEBUG("[DEBUF] LOGGER task received HEARTBEAT request.\n");
					  log_msg = create_message_struct(LOGGER_THREAD, MAINTHREAD, HEARTBEAT, HEART_BEAT);
					  info.data = log_msg;
					  info.thread_mutex_lock = main_queue_mutex;
					  info.qName = MAIN_QUEUE;
					  status = msg_send(&info);
            DEBUG("[DEBUG] LOGGER task responded to HEARTBEAT request.\n");
					  break;
				  case SHUT_DOWN:
            DEBUG("[DEBUF] LOGGER task received SHUTDOWN request.\n");
            status = ERROR;
            break; //EXIT CODE
				  default:;
			  }
      }
    }
#endif
    sleep(5);
	}
  /* Clean up */
  fclose(pfile);
  pthread_mutex_destroy(&log_queue_mutex);
  mq_unlink(LOGGER_QUEUE);
  pthread_exit(NULL);
}

Status_t log_data(FILE **pfile, Message_t *message, const char *fileName)
{
	if(*pfile == NULL)	return ERROR;
	if(message == NULL)	return ERROR;

	if((*pfile=fopen(fileName,"a+")) == NULL)
	{
		DEBUG("Error in Creating/Opening File\n");
		return ERROR;
	}

	char logData[100] = {(uint8_t)'\0'};
	sprintf(logData, "Task: %s\tTime: %s\n%s: %s\n\n",
		task[message->sourceId], ctime(&message->timeStamp),
		levels[message->type], message->msg);

	if(fwrite(logData, strlen(logData), 1, *pfile) < 0)
	{
		DEBUG("Error in Writing Data");
		return ERROR;
	}

	fclose(*pfile);
	return SUCCESS;
}
