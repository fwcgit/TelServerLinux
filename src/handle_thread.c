//
//  handle_thread.c
//  TelServer
//
//  Created by fwc on 2018/5/28.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "h_thread.h"
#include "client_info.h"
#include "errno.h"
#include "client_table.h"
#include <time.h>
#include "j_callback.h"
#include "log.h"

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
    ListNode *node      = NULL;
    int ret             = 0;
    
    
    node= poll_list(list);
   
    while(is_run())
    { 
        usleep(10 * 1000);
        
        node = poll_list(list);
        if(node != NULL)
        {
            pk = (package *)node->data;
            char *key = pk->head.key;
            //printf("handlder key %s \r\n",key);
			switch (pk->head.type) {
				case MSG_TYPE_ID:
                    //printf("MSG_TYPE_ID\r\n");
                    clear_exist_client(key);
                    save_client(pk->fd, key);
                    client_online(key);
					break;
				case MSG_TYPE_CMD:
					break;
                case MSG_TYPE_DATA:
                    break;
				case MSG_TYPE_HEART:
                    ret = sync_heartbeat_handle(key);
                   if(ret == 0)
                   {
                       sync_heartbeat_handle(key);
                   }
                    
                    log_flush("recv heartbeat %s \n",key);
					break;
			}
			
		//	printf("handle msg fd %d--------type:%d---len:%d----buff %s \n",
		//		   pk->fd,pk->head.type,pk->head.len,pk->body);

            free(pk->data);
        	free(pk);
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

