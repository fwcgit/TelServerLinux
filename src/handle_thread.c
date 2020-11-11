//
//  handle_thread.c
//  TelServer
//
//  Created by fwc on 2018/5/28.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "h_thread.h"
#include "errno.h"
#include "hmsglist.h"
#include <time.h>
#include "j_callback.h"
#include "log.h"
char *hdata = "0";
void get_time_str(char *buff)
{
    time_t      rawtime;
    struct tm * timeinfo = NULL;
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (buff,30,"Now is %Y/%m/%d %H:%M:%S",timeinfo);
    
}

void* handle_msg(void *args)
{
    package *pk         = NULL;
    int ret             = 0;
       
    while(is_run())
    { 
        usleep(10 * 1000);
        pk = (package *)get_msg();
        if(pk != NULL)
        {
            char *key = pk->head.key;
            //printf("handlder key %s \r\n",key);
			switch (pk->head.type) {
				case MSG_TYPE_ID:
                    accept_new_user(pk->fd,key);
					break;
				case MSG_TYPE_CMD:
                    rece_user_data(key,pk->data,pk->head.len);
					break;
                case MSG_TYPE_DATA:
                    rece_user_str(key,pk->data,pk->head.len);
                    break;
                case MSG_TYPE_TRANSPOND:
                    break;
				case MSG_TYPE_HEART:
                    user_heartbeat(pk->fd,key);
                    send_data_pack(pk->fd,MSG_TYPE_HEART,hdata,1);                   
                    log_flush("recv heartbeat %s \n",key);
					break;
			}
			
		//	printf("handle msg fd %d--------type:%d---len:%d----buff %s \n",
		// pk->fd,pk->head.type,pk->head.len,pk->body);

        	free(pk);
            pk = NULL;
        }
    }
    
    return(void*)NULL;
}

void start_handle_thread(void)
{
	pthread_t pid;
	int ret;

	ret = pthread_create(&pid,NULL,handle_msg,NULL);

	if(ret == 0)
	{
		log_flush("start_handle_thread success\n");
	}
	else
	{
		log_flush("start_handle_thread fail\n");
	}
}

