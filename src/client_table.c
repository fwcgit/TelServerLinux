//
//  cleint_table.c
//  TelServer
//
//  Created by fwc on 2018/8/27.
//  Copyright © 2018年 fwc. All rights reserved.
//

#include "client_table.h"
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "j_callback.h"
#include "map.h"
#include "server.h"

root_t tree = RB_ROOT;
fd_set read_set;
void **table = NULL;
void **authtable = NULL;
int curr_count = 0;
int curr_auth_count = 0;


void fresh_table()
{
    int count = 0;
    int auth_count = 0;
    map_t *node;
    for (node = map_first(&tree); node; node = map_next(&(node->node)))
    {
        count++;
        client_info *ci = (client_info *)node->val;
        printf("Node Key %s %d\r\n",ci->code,ci->fd);
    }
    if (count > 0)
    {
        if(table) free(table);
        if(authtable) free(authtable);


        table = (void **)malloc(sizeof(void *) * count);
        authtable = (void **)malloc(sizeof(void *) * count);
        int i = 0;
       
        for (node = map_first(&tree); node; node = map_next(&(node->node)))
        {
            client_info *ci = (client_info *)node->val;
            if (ci->isAuth)
            {
                *(authtable + auth_count) = ci;
                auth_count++;
            }
                *(table + i) = ci;
                i++;
            
        }
    }
    else
    {
        if(table) free(table);
        if(authtable) free(authtable);

        table = NULL;
        authtable = NULL;
    }
    

    curr_auth_count = auth_count-1;
    curr_count = count-1;


    printf("fresh_table curr_auth_count %d curr_count %d \r\n",curr_auth_count,curr_count);
}
/***
 同步读取客户端列表
 **/
void **sync_read_mapclient_list(int *size, char isAuth)
{    
    if (isAuth)
    {
        *size = curr_auth_count;
        return authtable;
    }
    
    *size = curr_count;
    return table;
}

/***
 同步查找没有认证的客户端
 **/
void sync_find_auth_timeout_client()
{
#if 1
    int index = 0;
    int c_len = 0;
    int i = 0;
    client_info *ci;
    time_t raw_time;
    int *fds;
    if (curr_count > 0)
    {
        time(&raw_time);
        fds = (int *)malloc(sizeof(int) * curr_count);
        for (i = 0; i < curr_count; i++)
        {
            ci = (client_info *)*(table + i);
            if(ci)
            {
                if (raw_time - ci->ctime >= 10 && !ci->isAuth)
                {
                    *(fds + c_len) = ci->fd;
                    c_len++;
                }
            }

        }
        if (c_len > 0)
        {
            sync_free_client(fds, c_len);
        }

        free(fds);
    }
#endif
}

/***
 同步移除列表中的客户端
 **/
int sync_remove_list_client(int fd)
{
    int ret = 0;
    map_t *data;
    int i;
    
    close(fd);
    
    data = get(&tree, (char *)&fd);
    if (data)
    {
        rb_erase(&data->node, &tree);
        map_free(data);
    }
    fresh_table();
    return 0;
}

/***
 同步批量释放客户端
 **/
int sync_free_client(int *fds, int len)
{

    int ret = 0;
    map_t *data;
    int i;

    for (i = 0; i < len; i++)
    {
        data = get(&tree, (char *)&(*(fds + i)));
        if (data)
        {
            rb_erase(&data->node, &tree);
            map_free(data);
        }
    }

     for (i = 0; i < len; i++)
    {
        close(*(fds + i));
    }
   
    fresh_table();
    return 0;
}

/***
 同步处理心跳次数
 **/
int sync_heartbeat_set(char *key)
{
    int ret = 0;
#if 1
    map_t *data;
    client_info *ci;
    data = get(&tree, key);
    if (data)
    {
        ci = (client_info *)data->val;
        if (ci->isAuth)
        {
            ci->ioTimeout++;
        }
    }
#endif
    return ret;
}

/***
 同步处理心跳
 **/
int sync_heartbeat_handle(char *key)
{
    int ret = 0;
#if 1
    map_t *data;
    client_info *ci;

    data = get(&tree, key);
    if (data)
    {
        ci = (client_info *)data->val;
        
        if (ci->isAuth)
        {
             send_data_pack(ci->fd,MSG_TYPE_HEART,"",0);
            ci->ioTimeout = 0;
        }
    }
#endif
    return ret;
}

/***
 同步获取客户端数量
 **/
int sync_get_client_count(void)
{

    int count = 0;

    return count;
}

void client_tbl_init(void)
{

#if 0
    client_info *ci = (client_info *)malloc(sizeof(client_info));
    ci->fd = 100;
    put(&tree,"123",ci);

    map_t *note = get(&tree,"123");
    client_info *ci2 = (client_info *)note->val;
    if(NULL != ci2)
    {
        printf("----%d\r\n",ci2->fd);
    }
    else
    {
        printf("%s NULL\r\n",note->key);
    }
#endif
}

int accept_client_tbl(int fd)
{
    int ret;
    time_t raw_time;
    time(&raw_time);
    client_info *ci;

    ci = (client_info*) malloc(sizeof(client_info));
    memset(ci, 0, sizeof(client_info));
    ci->fd = fd;
    ci->ctime = raw_time;
    put(&tree, (char *)&(ci->fd), ci);
    printf("accept_client_tbl fd=%d raw_time %ld\r\n", ci->fd,ci->ctime);
    fresh_table();
    return 0;
}

void add_fd_set()
{
    int i = 0;
    time_t raw_time;
    time(&raw_time);
    client_info *ci = NULL;
    if (NULL != table)
    {
        for (i = 0; i < curr_count; i++)
        {
            ci = (client_info *)(*(table + i));
            if (NULL == ci)
            {
                printf("ci = NULL \r\n");
            }
            else
            {
                FD_SET(ci->fd, &read_set);
            }
        }
    }

   // printf("++++++++++++++++++++++++++++++++++++++++add_fd_set raw_time %ld \r\n ",raw_time);
}

int find_max_fd()
{
    int i = 0;
    client_info *ci = NULL;
    int maxfd = 0;
    if (NULL != table)
    {
        for (i = 0; i < curr_count; i++)
        {
            ci = (client_info *)(*(table + i));
            if (NULL != ci)
            {
                if (maxfd < ci->fd)
                    maxfd = ci->fd;
            }
            else
            {
                printf("find_max_fd NULL \r\n");
            }
        }
    }

    return maxfd;
}

void force_client_close(client_info *ci)
{
    map_t *data;
    int fd;
    if (NULL != ci)
    {
        printf("force_client_close %s %d \r\n", ci->code, ci->fd);
        fd = ci->fd;
        if(fd > 3)
        {
            close(fd);
        }
        data = get(&tree, ci->code);
        if (data)
        {
            rb_erase(&(data->node), &tree);
            map_free(data);
            client_off_line(ci->code);
            send_user("M0001",MSG_TYPE_CMD,"change",6);
        }

        data = get(&tree, (char *)&fd);
        if (data)
        {
            rb_erase(&(data->node), &tree);
            map_free(data);
        } 
    }
    fresh_table();
}

void clear_exist_client(char *key)
{
    int ret;
    map_t *data;
    client_info *ci;

    data = get(&tree, key);
    if (data)
    {
        ci = (client_info *)data->val;
        if (NULL != ci)
        {
            force_client_close(ci);
        }
    }
    
}
void save_client(int fd, char *key)
{
    int ret;
    client_info *newci;
    client_info *p;
    map_t *data;

    printf("auth success %s %d \r\n", key, fd);

    data = get(&tree, (char *)&(fd));
    if (data)
    {
        p = (client_info *)data->val;
        newci = (client_info *)malloc(sizeof(client_info));
        newci->fd = fd;
        newci->isAuth = 1;
        memset(newci->code,0,sizeof(newci->code));
        strcpy(newci->code, key);
        if(p)
        {
            newci->ctime = p->ctime;
        }
        put(&tree, key, newci);
        printf("<auth success> %s %d \r\n", newci->code, newci->fd);
        rb_erase(&(data->node), &tree);
        map_free(data);

        client_info *ti = get_client(key);
        if(ti)
        {
        printf("---------------------------------<auth success> <------->{%s} %d \r\n", ti->code, ti->fd);

        }
        else
        {
            
        printf("--------------------------------------<auth success> <get_client> \r\n");

        }
    }

      fresh_table();
}

client_info *client_list(int *count)
{
    int size = 0;
    int i;
    client_info **table = (client_info **)sync_read_mapclient_list(&size, 1);
    if (size == 0)
        return NULL;
    client_info *list = (client_info *)malloc(sizeof(client_info) * size);
    if (NULL != table)
    {
        *count = size;
        for (i = 0; i < size; i++)
        {
            *(list + i) = *(*(table + i));
        }
    }

    return list;
}

client_info *get_client(char *session)
{

    int ret;
    map_t *node;
    client_info *ci = NULL;
    node = get(&tree, session);
    if (NULL != node)
    {
        ci = (client_info *)node->val;
        if (NULL != ci)
        {
            printf("get_client %s %d \r\n", ci->code, ci->fd);
        }
        else
        {
           printf("get_client client_info NULL");
        }
        
    }
    else
    {
        printf("get_client Node NULL");
    }
    

    return ci;
}
