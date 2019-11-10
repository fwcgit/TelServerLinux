//
//  check_authclient_thread.c
//  TelServer
//
//  Created by fwc on 2018/8/2.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include <stdio.h>
#include "h_thread.h"
#include <unistd.h>
#include "server.h"
#include "socket.h"
#include <string.h>
#include <stdlib.h>
#include "client_table.h"
#include "log.h"
void* run_auth_client(void *args)
{
    while(is_run())
    {
        sleep(10);
        sync_find_auth_timeout_client();    
        fflush(stdout);
        
    }
    return (void *)NULL;
}

void start_auth_thread(void)
{
	pthread_t pid;
	int ret;

	ret = pthread_create(&pid,NULL,run_auth_client,NULL);

	if(ret == 0)
	{
		log_flush("start_auth_thread success\n");
	}
	else
	{
		log_flush("start_auth_thread fail\n");
	}

}
