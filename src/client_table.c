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

pthread_rwlock_t rw_lock;

root_t tree = RB_ROOT;
fd_set read_set;

/***
 同步读取客户端列表
 **/
void **sync_read_mapclient_list(int *size, char isAuth)
{
    int i = 0;
    void *obj = NULL;
    client_info *ci = NULL;
    void **table = NULL;
    int ret = -1;
    int count = 0;
    int auth_count = 0;
    ret = pthread_rwlock_rdlock(&rw_lock);

    if (ret == 0)
    {
        map_t *node;
        for (node = map_first(&tree); node; node = map_next(&(node->node)))
        {
            count++;
            ci = (client_info *)node->val;
        }
        if (count > 0)
        {
            table = (void **)malloc(sizeof(void *) * count);
            i = 0;
            for (node = map_first(&tree); node; node = map_next(&(node->node)))
            {
                ci = (client_info *)node->val;
                if (isAuth && ci->isAuth)
                {
                    *(table + auth_count) = ci;
                    auth_count++;
                }
                else
                {
                    *(table + i) = ci;
                    i++;
                }
            }
        }

        pthread_rwlock_unlock(&rw_lock);
    }
    else
    {
        printf("sync_read_mapclient_list rdlock fail\n");
        return NULL;
    }
    if (isAuth)
    {
        *size = auth_count;
    }
    else
    {
        *size = count;
    }

    return table;
}

/***
 同步查找没有认证的客户端
 **/
void sync_find_auth_timeout_client()
{
#if 1
    int index = 0;
    int count = 0;
    int c_len = 0;
    int i = 0;
    client_info *ci;
    time_t raw_time;
    int *fds;
    void **table = sync_read_mapclient_list(&count, 0);
    if (count > 0)
    {
        time(&raw_time);
        fds = (int *)malloc(sizeof(int) * count);
        for (i = 0; i < count; i++)
        {
            ci = (client_info *)*(table + i);
            if (raw_time - ci->ctime >= 10 && !ci->isAuth)
            {
                *(fds + c_len) = ci->fd;
                c_len++;
            }
        }
        if (c_len > 0)
        {
            sync_free_client(fds, c_len);
        }

        free(fds);
        free(table);
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
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {

        data = get(&tree, (char *)&fd);
        if (data)
        {
            rb_erase(&data->node, &tree);
            map_free(data);
        }
    }
    pthread_rwlock_unlock(&rw_lock);
    return 0;
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
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {
        for (i = 0; i < len; i++)
        {
            data = get(&tree, (char *)&(*(fds + i)));
            if (data)
            {
                rb_erase(&data->node, &tree);
                map_free(data);
            }
        }
    }
    pthread_rwlock_unlock(&rw_lock);

     for (i = 0; i < len; i++)
    {
        close(*(fds + i));
    }
   
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
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {
        data = get(&tree, key);
        if (data)
        {
            ci = (client_info *)data->val;
            if (ci->isAuth)
            {
                ci->ioTimeout++;
            }
        }
    }
    pthread_rwlock_unlock(&rw_lock);
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
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {
        data = get(&tree, key);
        if (data)
        {
            ci = (client_info *)data->val;
            if (ci->isAuth)
            {
                ci->ioTimeout = 0;
            }
        }
    }
    pthread_rwlock_unlock(&rw_lock);
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
    pthread_rwlock_init(&rw_lock, NULL);
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
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {
        ci = (client_info*) malloc(sizeof(client_info));
        memset(ci, 0, sizeof(client_info));
        ci->fd = fd;
        ci->ctime = raw_time;
        put(&tree, (char *)&(ci->fd), ci);
        printf("accept_client_tbl fd=%d raw_time %ld\r\n", ci->fd,ci->ctime);
        pthread_rwlock_unlock(&rw_lock);
    }
    else
    {
        printf("accept client_lock_fail..\r\n");
    }

    return 0;
}

void add_fd_set()
{
    int i = 0;
    client_info *ci = NULL;
    void **tableClient = NULL;
    int count = 0;
    tableClient = sync_read_mapclient_list(&count, 0);
    if (NULL != tableClient)
    {
        for (i = 0; i < count; i++)
        {
            ci = (client_info *)(*(tableClient + i));
            if (NULL == ci)
            {
                printf("ci = NULL \r\n");
            }
            else
            {
                FD_SET(ci->fd, &read_set);
            }
        }
        free(tableClient);
        tableClient = NULL;
    }
}

int find_max_fd()
{
    int i = 0;
    client_info *ci = NULL;
    void **tableClient = NULL;
    int maxfd = 0;
    int count = 0;

    tableClient = sync_read_mapclient_list(&count, 0);

    if (NULL != tableClient)
    {
        for (i = 0; i < count; i++)
        {
            ci = (client_info *)(*(tableClient + i));
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

        free(tableClient);
        tableClient = NULL;
    }

    return maxfd;
}

void force_client_close(client_info *ci)
{
    int ret;
    map_t *data;
    int fd;
    printf("force_client_close %s %d \r\n", ci->code, ci->fd);
    close(ci->fd);
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {
        if (NULL != ci)
        {
            fd = ci->fd;
            data = get(&tree, ci->code);
            if (data)
            {
                rb_erase(&(data->node), &tree);
                map_free(data);
                client_off_line(ci->code);
            }

            data = get(&tree, (char *)&fd);
            if (data)
            {
                rb_erase(&(data->node), &tree);
                map_free(data);
            } 
        }
    }
    pthread_rwlock_unlock(&rw_lock);
}

void clear_exist_client(char *key)
{
    int ret;
    map_t *data;
    client_info *ci;
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {
        data = get(&tree, key);
        if (data)
        {
            ci = (client_info *)data->val;
            pthread_rwlock_unlock(&rw_lock);
            if (NULL != ci)
            {
                force_client_close(ci);
            }
        }
        else
        {
            pthread_rwlock_unlock(&rw_lock);
        }
    }
}
void save_client(int fd, char *key)
{
    int ret;
    client_info *newci;
    client_info *p;
    map_t *data;
    ret = pthread_rwlock_wrlock(&rw_lock);
    if (ret == 0)
    {
        printf("auth success %s %d \r\n", key, fd);
    
        data = get(&tree, (char *)&(fd));
        if (data)
        {
            p = (client_info *)data->val;
            newci = (client_info *)malloc(sizeof(client_info));
            newci->fd = fd;
            newci->isAuth = 1;
            strcpy(newci->code, key);
            if(p)
            {
                newci->ctime = p->ctime;
            }
            put(&tree, key, newci);

            rb_erase(&(data->node), &tree);
            map_free(data);
        }
    }
    pthread_rwlock_unlock(&rw_lock);
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
    ret = pthread_rwlock_rdlock(&rw_lock);
    if (ret == 0)
    {
        node = get(&tree, session);
        if (NULL != node)
        {
            ci = (client_info *)node->val;
            if (NULL != ci)
            {
                printf("get_client %s %d \r\n", ci->code, ci->fd);
            }
        }
    }
    pthread_rwlock_unlock(&rw_lock);
    return ci;
}
