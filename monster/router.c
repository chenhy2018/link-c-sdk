//
// Created by chenh on 2019/1/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "router.h"

//5 个线程进行请求处理
static int router_in()
{
    //接收消息
    //推队列

}

static int router_out()
{
    //取消息队列消息
    //发送消息
}

int init_router()
{

}

struct RouterObj *NewRouterObj()
{
    struct RouterObj *routerObj = (struct RouterObj *)DevMalloc(sizeof(struct RouterObj));

}

void DelRouterObj(struct RouterObj *obj)
{
    //结束线程
    //销毁资源
    DevFree(obj);
}
