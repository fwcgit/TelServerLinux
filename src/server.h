//
//  server.h
//  TelServer
//
//  Created by fwc on 2018/5/25.
//  Copyright © 2018年 fwc. All rights reserved.
//

#ifndef server_h
#define server_h
#include "socket.h"
#include "h_thread.h"
#include "client_info.h"
#define MAX_CLIENT 1024
extern unsigned int client_count;
extern int fds[MAX_CLIENT];
extern int fds_cnt;
extern fd_set read_set;
#ifdef __cplusplus
extern "C" {
#endif

void starp_server(void);

void init_config(int port);

void init(void);

void stop_server(void);

ssize_t send_data_pack(int fd,char type,char *data,size_t len);

ssize_t send_data(int fd,char type,char *data,size_t len);

void pack_data(char *data,void *msg,size_t m_len,char *src,size_t s_len);

void rece_user_data(char *key,char *data,size_t len);

void rece_user_str(char *key,char *data,size_t len);

int find_max_fd(void);

void new_user_connect(int fd);

void accept_new_user(int fd,char *key);

void user_heartbeat(int fd,char *key);

void client_disconnect(int fd);

#ifdef __cplusplus
}
#endif
#endif /* server_h */
