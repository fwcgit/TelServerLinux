//
//  h_thread.c
//  TelServer
//
//  Created by fwc on 2018/5/25.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "h_thread.h"

unsigned char run = 0;
List *list;

pthread_t accept_pid;
pthread_t read_pid;
pthread_t handle_pid;
pthread_t auth_pid;
pthread_t heart_pid;
unsigned char is_run(void)
{
    return run;
}

void start_thread(int sockfd)
{
    run = 1;
    start_accept_thread();
	start_read_thread();
	start_handle_thread();
}

void stop_thread(void)
{
    run = 0;
}

