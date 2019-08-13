//
//  read_thread.c
//  TelServer
//
//  Created by fwc on 2018/5/25.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "h_thread.h"
#include "client_table.h"
#include "crc.h"
void* read_client(void *args)
{
    int i                       = 0;
    int maxfd                   = 0;
    int ret                     = 0;
    ssize_t rec                 = 0;
    ssize_t totalBytes          = 0;
    ssize_t data_len            = 0;
    unsigned long packageLen    = 0;
    char buff[1024];
    
    struct timeval tv;
    
    int count;
    
    tv.tv_sec = 1;
    tv.tv_usec = 500;
    
    while(is_run())
    {
        char *data;
        client_info *info           = NULL;
        void **tableClient          = NULL;

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
            perror("select read fail ! \n");
        }
        else if(ret == 0)
        {
           // printf("select read time out! \n");
        }
        else
        {
            tableClient = sync_read_mapclient_list(&count,0);
            
            if(NULL != tableClient)
            {
                for(i = 0 ; i < count ;i++)
                {
                     info = (client_info *)(*(tableClient+i));
                    if(FD_ISSET(info->fd,&read_set))
                    {
                        memset(&buff, 0, sizeof(buff));
                        rec = recv(info->fd, buff, 8, 0); 
                        if(rec > 0)
                        {
                            totalBytes      += rec;
                            while(totalBytes < 8)
                            {
                                rec = recv(info->fd, buff, 1, 0);
                                if(rec <= 0)
                                {
                                    printf("read data fail 1 %ld \r\n",rec);
                                    force_client_close(info);
                                    break;
                                } 
                                totalBytes  += rec;
                            }

                            if(rec == 8)
                            {
                                if(*buff == (unsigned char)0x3b)
                                {
                                    uint16 crc = *(buff+6) & 0x00ff;
                                    crc <<= 8;
                                    crc |= *(buff+7);

                                    uint16 code_crc = CRC16(buff,8-2);
                                    if(crc == code_crc)
                                    {
                                        data_len =  *(buff+1) & 0x000000ff;
                                        data_len <<= 8;
                                        data_len |= *(buff+2);
                                        data_len <<= 8;
                                        data_len |= *(buff+3);
                                        data_len <<= 8;
                                        data_len |= *(buff+4);

                                        while(totalBytes < data_len)
                                        {
                                             rec = recv(info->fd, buff, data_len, 0);
                                            if(rec <= 0)
                                            {
                                                printf("read data fail 1 %ld \r\n",rec);
                                                force_client_close(info);
                                                break;
                                            } 
                                            totalBytes  += rec;
                                        }

                                        if(totalBytes == data_len)
                                        {
                                            char *data = (char *)malloc(sizeof(char) * data_len - 8);
                                            package *pk = (package *)malloc(sizeof(package));
                                            memcpy(data,buff+8,data_len-8);
                                            pk->head.type = *(buff+5);
                                            pk->fd    = info->fd;
                                            pk->data  = data;
                                            add_list(list, pk);
                                            printf("recv %s Len:%ld\n",data,packageLen);
                                        }
                                    }
                                }
                            }
                        }
                        else if(rec <= 0)
                        {
                            printf("read data fail 2  %ld \r\n",rec);
                            force_client_close(info);
                            
                        }
                    }
                }
                
                free(tableClient);
                tableClient = NULL;
            }
        }
    }
    return (void*)NULL;
}

void start_read_thread(void)
{
	pthread_t pid;
	int ret;

	ret = pthread_create(&pid,NULL,read_client,NULL);

	if(ret == 0)
	{
		printf("start_read_thread success\n");
	}
	else
	{
		perror("start_read_thread fail \n");
	}
}
