//
//  cleint_table.h
//  TelServer
//
//  Created by fwc on 2018/8/27.
//  Copyright © 2018年 fwc. All rights reserved.
//

#ifndef cleint_table_h
#define cleint_table_h

#include <sys/select.h>
#include "client_info.h"

extern fd_set read_set;
void client_tbl_init(void);

int accept_client_tbl(int fd);

int  find_max_fd(void);

void add_fd_set(void);

void force_client_close(client_info *ci);

void clear_exist_client(char *key);

void save_client(int fd,char *key);

client_info *client_list(int *count);

client_info *get_client(char *session);

/**------------------**/
void ** sync_read_mapclient_list(int *size,char isAuth);

void sync_find_auth_timeout_client();

int sync_get_client_count(void);

int sync_remove_list_client(int fd);

int sync_heartbeat_handle(char *key);

int sync_heartbeat_set(char *key);

int sync_free_client(int *fds,int len);

int *get_fd_list(int *count);

#endif /* cleint_table_h */
