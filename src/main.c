//
//  main.c
//  TelServer
//
//  Created by fwc on 2018/5/24.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "list.h"
#include<string.h>
#include <sys/time.h>
#include "server.h"
#include "client_info.h"
#include "crc.h"
#include <stdarg.h>
#include <limits.h>

unsigned int client_count = 0;
char *key = "Print0001";


void my_printf(const char *format,...)
{
    va_list args;
    
    va_start(args,format);
    vprintf(format,args); //必须用vprintf
    va_end(args);
    fflush(stdout);
}

char* int_to_str(int val)
{
    char *str = (char *)malloc(sizeof(char)*10);
    sprintf(str,"%d",val);
    free(str);
    return str;
}

void send_test_data(void)
{
    char text[4096 * 10];
    memset(text, 'C', 4096 * 10);
    printf("send_user\r\n");
    send_user(key, MSG_TYPE_DATA,text, 4096 * 10);
}

void send_txt_data(void)
{
    char text[10];
    memset(text, 'A',10);
    printf("send_user\r\n");
    send_user(key, MSG_TYPE_DATA,text, 10);
}

int main(int argc, const char * argv[]) {

//client_tbl_init();
void **tab = (void **)malloc(sizeof(void *) * 10);
client_info *ci1 = (client_info *)malloc(sizeof(client_info));
ci1->fd = 100;
client_info *ci2 = (client_info *)malloc(sizeof(client_info));
ci2->fd = 102;
 *(tab+0) = ci1;
 *(tab+1) = ci2;  

 client_info *pc = (client_info *)*(tab+0); 
 printf("main %d \n",pc->fd);
 pc = (client_info *)*(tab+1); 
 printf("main %d \n",pc->fd);
#if 1
	client_info  *table;
	int count,i;
    char sessio[100];
    
	init_config(38888);
    starp_server();

    while(1)
    {
        gets(sessio);

        if(strcmp(sessio,"q") == 0)
        {
			stop_server();
            exit(0);
        }
        else if(strstr(sessio,"send"))
        {
            //send_test_data();
            send_txt_data();
        }
		else if(strstr(sessio,"ls"))
		{
			printf("get_client_list\n");
            count = 0;
			table = get_client_list(&count);
			if(count > 0)
			{
				for(i = 0 ; i < count ;i++)
				{				
					printf("%d-client_id code:%s\n",i+1,(table+i)->code);
				}
				free(table);
			}

			
		}
    }
#endif

#if 0
    char buff[4];
    buff[0] = 'a';
    buff[1] = 'b';
    buff[2] = 'c';
    buff[3] = 'd';
    
    char *pc;
    
    package *pk = (package*)malloc(sizeof(package));
    pk->head.type = 1;
    pk->head.len = 10;
    pk->fd = 101;
    pk->data = malloc(sizeof(char)*4);
    memcpy(pk->data, "buff", 4);
    
    //pc = (char *)pk->data;
    printf("%ld\r\n",sizeof(*pk));
#endif
	return 0;
}
