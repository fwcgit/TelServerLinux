//
//  accept_thread.c
//  TelServer
//
//  Created by fwc on 2018/5/25.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "h_thread.h"
#include "log.h"
#include "client_table.h"

void* accept_client(void *args)
{
    char *reqCode = "reqCode";
    fd_set rec_fd_set;
    int new_fd;
    struct timeval tv;
    sockaddr_IN client_in;
    socklen_t client_len_t;
    int ret = 1;
    tv.tv_sec = 0;
    tv.tv_usec = 500;
    
    while (is_run())
    {
        FD_ZERO(&rec_fd_set);
        FD_SET(sockFD,&rec_fd_set);
        
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
        ret = select(sockFD+1,&rec_fd_set,NULL,NULL,&tv);
        
        if(ret < 0)
        {
            perror("accept_client select error \n");
        }
        else if(ret == 0)
        {

           // printf("accept_client select timeout \n");
        }
        else
        {
            client_len_t = sizeof(client_in);
            new_fd = accept(sockFD, (sockAddr *)&client_in, &client_len_t);
            
            if(new_fd <=0 )
            {
                printf("accept client error \n");
                continue;
            }
 
            accept_client_tbl(new_fd);
            
            send_data_pack(new_fd, MSG_TYPE_ID,reqCode, strlen(reqCode));
            
            printf("connected client ip:%s new_fd:%d ret:%d \n", inet_ntoa(client_in.sin_addr),new_fd,ret);
		
        }
	
    } 
    return (void*)NULL;
}

void start_accept_thread(void)
{
	int ret;
	pthread_t pid;
		
	ret = pthread_create(&pid,NULL,accept_client,NULL);
	if(ret == 0)
	{
		printf("start_accept_thread success\n");
	}
	else
	{
		perror("start_accept_thread fail\n");
	}
}
