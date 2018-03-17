#ifndef _TASK_H__
#define _TASK_H__


#define TMP_PERIOD	(2000)
#define RETRY_MAX   (3)

#define PORT_NO (9999)

void * task_tmp(void * param);
void * task_light(void * param);
void * task_socket(void * param);
void * task_log(void * param);

int32_t tmp1021_init(int32_t *dev_fp);
int32_t apds9301_init(int32_t *dev_fp);



#endif
