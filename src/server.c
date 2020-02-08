//
//  server.c
//  TelServer
//
//  Created by fwc on 2018/5/25.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "server.h"
#include "client_info.h"
#include "client_table.h"
#include <time.h>
#include "crc.h"
#include <signal.h>
int LISTENER_PORT;

void starp_server(void)
{
    signal(SIGPIPE,SIG_IGN);
    client_tbl_init();
    
    int fd = listener_socket();
    if(fd > 0)
    {
      start_thread(fd);
    }
    
}

void init_config(int port)
{
	LISTENER_PORT = port;
}

void init(void)
{
	LISTENER_PORT = 38888;
}

void printOldData(char *data,ssize_t len)
{
#if 1
    int i = 0;
    if(NULL != data && len > 0){
        for(i = 0; i < len; i++)
        {
                log_flush("%02X--",*(data+i));
        }

        log_flush("\r\n");
    }
 #endif
}

ssize_t send_data_pack(int fd,char type,char *data,size_t len)
{
        int data_len = FRAME_HEAD_SIZE+2+len;
        char *user_data = (char *)malloc(sizeof(char) * data_len);
        *user_data = 0x3b;
        *(user_data+1) = (char)(data_len >> 24);
        *(user_data+2) = (char)(data_len >> 16);
        *(user_data+3) = (char)(data_len >> 8);
        *(user_data+4) = (char)(data_len >> 0);
        *(user_data+5) = type;

        uint16_t crc_code = CRC16((unsigned char *)user_data,6);
        *(user_data+6) = (char)(crc_code>>8);
        *(user_data+7) = (char)(crc_code>>0);

        uint16_t crc_data = CRC16((unsigned char *)data,len);
        *(user_data+(data_len-2)) = (char)(crc_data>>8);
        *(user_data+(data_len-1)) = (char)(crc_data>>0);

        memcpy(user_data+FRAME_HEAD_SIZE,data,len);
        printOldData(user_data,data_len);
        ssize_t s_len = send_data(fd,type,user_data,data_len);
        return s_len;
}
ssize_t send_data(int fd,char type,char *data,size_t len)
{
    
    ssize_t s_len;
    int ret;
    fd_set wfd;
    struct timeval tv;
    
    FD_ZERO(&wfd);
    FD_SET(fd,&wfd);
    tv.tv_sec = 0;
    tv.tv_usec = 100 * 1000;
    
    ret = select(fd+1,NULL , &wfd,NULL,&tv);
    
    if(ret == 0)
    {
        
    }
    else if(ret < 0 )
    {
        
    }
    else
    {
        s_len = write(fd, data, len);
        
        if(s_len == 0)
        {
            log_flush("send data fail %d\n",fd);
            return 0;
        }
        
        return s_len;
    }
    
    return ret;
}

ssize_t send_user(char *session,char type,char *data,size_t len)
{
    client_info *ci = get_client(session);
    ssize_t ret = 0;
    if(NULL != ci)
    {
        log_flush("send user client %s \n",ci->code);
        ret = send_data_pack(ci->fd,type, data, len);
    }
    else
    {
        ret = -1;
        log_flush("send user no client %s\n",session);
    }
    
    return ret;
}

client_info *get_client_list(int *count)
{
	return client_list(count);
}

void stop_server(void)
{
    stop_thread();
    close_socket();
}

void pack_data(char *data,void *msg,size_t m_len,char *src,size_t s_len)
{
    if(NULL == data || NULL == msg || NULL == src) return;
    
    memcpy(data, msg, m_len);
    memcpy(data+m_len, src, s_len);
}
