//
//  j_callback.h
//  TelServer
//
//  Created by fwc on 2018/9/3.
//  Copyright © 2018年 fwc. All rights reserved.
//

#ifndef j_callback_h
#define j_callback_h
#ifdef __cplusplus
extern "C" {
#endif
void client_online(char *session);

void client_off_line(char *session);
#ifdef __cplusplus
}
#endif
#endif /* j_callback_h */
