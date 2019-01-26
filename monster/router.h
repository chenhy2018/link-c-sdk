//
// Created by chenh on 2019/1/23.
//

#ifndef LINK_C_SDK_ROUTER_H
#define LINK_C_SDK_ROUTER_H

#include "connect.h"

typedef int (*fn_Deal)(unsigned int cmd, void *args);
enum RouterStatus {
    ROUTER_UNINIT = 0,
    ROUTER_START = 0,
    ROUTER_STOP
};

struct RouterDealIteam {
    unsigned int cmd;
    void (*Deal)(unsigned int cmd, void *args);
};

struct RouterOperation {
    void (*RegisterDealItem)(unsigned int cmd, fn_Deal func);
    void (*UnRegisterDealItem)(unsigned int cmd);
};

struct RouterObj {
	struct ConnectObj *stConnObj;
	int nCount;
	struct RouterDealIteam stRouterDealTab[12];
	struct RouterOperation *stOpt;
};


enum MQTT_IOCTRL_CMD {
    MQTT_IOCTRL_OFFSET = 0,
    MQTT_IOCTRL_SETLOGLEVEL,
    MQTT_IOCTRL_GETLOGLEVEL
};

#endif //LINK_C_SDK_ROUTER_H
