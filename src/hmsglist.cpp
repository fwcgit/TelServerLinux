#include "hmsglist.h"
#include <stdlib.h>
#include "log.h"
#include <queue>
using namespace std;
queue<package*> msglist;
void add_msg(package *pk){
     log_flush("add_msg\r\n");
    msglist.push(pk);
}

void* get_msg()
{
    package *pk = NULL;
    if(msglist.empty()){
        return pk;
    }else{
        pk = msglist.front();
        msglist.pop();
    }
    return pk;
}