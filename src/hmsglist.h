#ifndef HMSGLIST_H_
#define HMSGLIST_H_
#include "msg.h"
#ifdef __cplusplus
extern "C" {
#endif


void add_msg(package *pk);

void* get_msg();

#ifdef __cplusplus
}
#endif
#endif