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
#include "crc.h"
#include <stdarg.h>
#include <limits.h>

unsigned int client_count = 0;
char *key = "00001";


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


int main(int argc, const char * argv[]) {

//client_tbl_init();

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
        }
        else if(strstr(sessio,"lar"))
        {
        }
        else if(strstr(sessio,"ir"))
        {
        }
		else if(strstr(sessio,"ls"))
        
		{
			
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
