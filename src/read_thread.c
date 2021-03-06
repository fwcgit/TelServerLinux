//
//  read_thread.c
//  TelServer
//
//  Created by fwc on 2018/5/25.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "h_thread.h"
#include "crc.h"
#include "log.h"
#include "server.h"
#include "hmsglist.h"
#define READ_WAIT_TIME  20 * 1000

void add_fd_set()
{
    time_t raw_time;
    time(&raw_time);

        for (int i = 0; i < fds_cnt; i++)
        {

        if(fds[i] > 0 && fds[i] < 1024)
        {
            FD_SET(fds[i], &read_set);
        }
            
        }
   // printf("++++++++++++++++++++++++++++++++++++++++add_fd_set raw_time %ld \r\n ",raw_time);
}

void* read_client(void *args)
{
    int maxfd                   = 0;
    int ret                     = 0;
    int s_fd                    = -1;
    ssize_t rec                 = 0;
    ssize_t totalBytes          = 0;
    ssize_t data_len            = 0;
    unsigned long packageLen    = 0;
    char buff[1024 * 1024];
    int recv_timout = 1000;
    struct timeval tv;
    
    int count;
    
    tv.tv_sec = 1;
    tv.tv_usec = 500;

    
    while(is_run())
    {
        char *data;
        FD_ZERO(&read_set);
        FD_SET(sockFD,&read_set);
        add_fd_set();
        maxfd = find_max_fd();
        maxfd = sockFD > maxfd ? sockFD : maxfd;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        ret = select(maxfd+1,&read_set,NULL,NULL,&tv);
        
        if(ret < 0 )
        {
            sleep(1);
            log_flush("select read fail ! \n");
        }
        else if(ret == 0)
        {
           //log_flush("select read time out! \n");
        }
        else
        {

                int retimout = 0;

                for(int i = 0 ; i < fds_cnt ;i++)
                {
                    s_fd = fds[i];
                    totalBytes = 0;
                    retimout = 0;
                    if(FD_ISSET(fds[i],&read_set))

                    {
                        memset(&buff, 0, sizeof(buff));
                        rec = recv(s_fd, buff, FRAME_HEAD_SIZE, MSG_DONTWAIT);
                        while(rec == 0 && retimout++ < 10)
                        {
                            usleep(READ_WAIT_TIME);
                            rec = recv(s_fd, buff, FRAME_HEAD_SIZE, MSG_DONTWAIT);
                        }
                        if(rec > 0)
                        {
                            totalBytes += rec;
                            while(totalBytes < FRAME_HEAD_SIZE)
                            {
                                rec = recv(s_fd, buff+totalBytes, 1, MSG_DONTWAIT);
                                retimout = 0;    
                                while(rec == 0 && retimout++ < 10)
                                {
                                    usleep(READ_WAIT_TIME);
                                    rec = recv(s_fd, buff+totalBytes, 1, MSG_DONTWAIT);
                                }

                                if(rec <= 0)
                                {
                                    break;
                                } 
                                totalBytes  += rec;
                            }

                            if(rec == FRAME_HEAD_SIZE)
                            {
                                if(*buff == 0x3b)
                                {
                                   
                                    uint16 crc = *(buff+6) & 0x00ff;
                                    crc <<= 8;
                                    crc |= *(buff+7)& 0x00ff;

                                    uint16 code_crc = CRC16((unsigned char *)buff,FRAME_HEAD_SIZE-2);
                                    if(crc == code_crc)
                                    {
                
                                       
                                        data_len =  *(buff+1) & 0x000000ff;
                                        data_len <<= 8;
                                        data_len |= *(buff+2)& 0x000000ff;
                                        data_len <<= 8;
                                        data_len |= *(buff+3)& 0x000000ff;
                                        data_len <<= 8;
                                        data_len |= *(buff+4)& 0x000000ff;

                                        while(totalBytes < data_len)
                                        {
                                            rec = recv(s_fd, buff+totalBytes, data_len, MSG_DONTWAIT);
                                            retimout = 0;    
                                            while(rec == 0 && retimout++ < 10)
                                            {
                                                usleep(READ_WAIT_TIME);
                                                rec = recv(s_fd, buff+totalBytes, data_len, MSG_DONTWAIT);
                                            }
                                            if(rec <= 0)
                                            {
                                                break;
                                            } 
                                            totalBytes  += rec;
                                        }

                                        if(totalBytes == data_len)
                                        {
                                            uint16 crc_data = *(buff+(data_len-2)) &0x00ff;
                                            crc_data <<= 8;
                                            crc_data |=  *(buff+(data_len-1))&0x00ff;
                                            int user_data_len = data_len -2-FRAME_HEAD_SIZE;
                                            uint16 crc_data_code = CRC16((unsigned char *)(buff+FRAME_HEAD_SIZE),user_data_len);
                                            if(crc_data == crc_data_code)
                                            {
                                                
                                                char *data = (char *)malloc(user_data_len);
                                                memset(data,0,user_data_len);
                                                memcpy(data,buff+FRAME_HEAD_SIZE,user_data_len);

                                                package *pk = (package *)malloc(sizeof(package));
                                                memset(pk,0,sizeof(package));
                                                
                                                pk->head.type = *(buff+5);
                                                pk->head.len = user_data_len;
                                                memset(pk->head.key,0,KEY_LEN+1);
                                                memcpy(pk->head.key,data,KEY_LEN);
                            
                                                pk->fd    = s_fd;
                                                pk->data  = data;
                                                add_msg(pk);
                                                
                                                int i = 0;
                                                
                                                for(i = 0; i < user_data_len; i++)
                                                {
                                                        log_flush("%02X--",*(data+i));
                                                }
                                                log_flush("--------------------\r\n");
                                                log_flush("recv %s Len:%ld\n",data,data_len);
                                            }

                                        }   
                                    }
                                }
                            }

                            log_flush("read data  %s \r\n",buff);
                        }
                        else if(rec <= 0)
                        {
                            client_disconnect(s_fd);
                            log_flush("read data fail 2  %ld \r\n",rec);
                            
                        }
                    }
                }
        }

    }

      log_flush("read exit\r\n");
    return (void*)NULL;
}

void start_read_thread(void)
{
	pthread_t pid;
	int ret;

	ret = pthread_create(&pid,NULL,read_client,NULL);

	if(ret == 0)
	{
		log_flush("start_read_thread success\n");
	}
	else
	{
		log_flush("start_read_thread fail \n");
	}
}
