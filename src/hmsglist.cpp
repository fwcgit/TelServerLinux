#include "hmsglist.h"
#include <queue>
using namespace std;
queue<package*> msglist;
void add_msg(package *pk){
    msglist.push(pk);
}

void* get_msg()
{
    if(msglist.empty()){
        return NULL;
    }else{
        return msglist.front();
    }
    return NULL;
}