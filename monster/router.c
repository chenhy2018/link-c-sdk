//
// Created by chenh on 2019/1/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "router.h"

static struct RouterObj gRouter = {};

static void RouterAssign(struct RouterObj *router, unsigned int cmd, char *args)
{
    switch (cmd) {
        case MQTT_IOCTRL_GETLOGLEVEL:
        case MQTT_IOCTRL_SETLOGLEVEL:
            if (router->stRouterDealTab[cmd - MQTT_IOCTRL_OFFSET].Deal) {
                router->stRouterDealTab[cmd - MQTT_IOCTRL_OFFSET].Deal(cmd, args);
            }
            break;
        default: {
            break;
        }
    }
}

void RouterRegisterDealIteam(unsigned int cmd, fn_Deal func, fn_Deal funcres)
{
    gRouter.stRouterDealTab[cmd - MQTT_IOCTRL_OFFSET].cmd = cmd;
    gRouter.stRouterDealTab[cmd - MQTT_IOCTRL_OFFSET].Deal = func;
}

void RouterUnRegisterDealItem(unsigned int cmd)
{
    gRouter.stRouterDealTab[cmd - MQTT_IOCTRL_OFFSET].cmd = -1;
    gRouter.stRouterDealTab[cmd - MQTT_IOCTRL_OFFSET].Deal = NULL;
}

struct RouterObj *NewRouterObj(struct ConnectObj *connObj)
{
    if (gRouter.stConnObj) {
        gRouter.nCount++;
        return &gRouter;
    }
    gRouter.nCount++;
    gRouter.stConnObj = connObj;
    return &gRouter;
}

void DelRouterObj(struct RouterObj *obj)
{
    obj->nCount--;
    if (!obj->nCount)
        obj->stConnObj = NULL;
}
