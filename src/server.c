//
//  server.c
//  TelServer
//
//  Created by fwc on 2018/5/25.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "server.h"
#include "client_info.h"
#include <time.h>
#include "crc.h"
int LISTENER_PORT;
int fds[MAX_CLIENT] = {-1};
int fds_cnt = 0;
fd_set read_set;

void starp_server(void)
{
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
                printf("%02X--",*(data+i));
        }

        printf("\r\n");
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
            printf("send data fail %d\n",fd);
            return 0;
        }
        
        return s_len;
    }
    
    return ret;
}

void stop_server(void)
{
    stop_thread();
    close_socket();
    for(int i = 0; i < fds_cnt; i++)
    {
        close(fds[i]);
    }
}

void pack_data(char *data,void *msg,size_t m_len,char *src,size_t s_len)
{
    if(NULL == data || NULL == msg || NULL == src) return;
    
    memcpy(data, msg, m_len);
    memcpy(data+m_len, src, s_len);
}

int find_max_fd(void)
{
    int max = 0;
    for(int i = 0; i < fds_cnt; i++)
    {
        if(max < fds[i])
            max = fds[i];
    }
    return max;
}
