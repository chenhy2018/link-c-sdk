//
// Created by chenh on 2019/1/23.
//

#ifndef LINK_C_SDK_ROUTER_H
#define LINK_C_SDK_ROUTER_H

#include "connect.h"

enum RouterStatus {
    ROUTER_UNINIT = 0,
    ROUTER_START = 0,
    ROUTER_STOP
};

struct RouterObj {
	struct ConnectObj stConnObj;
	pthread_t thId;
};


#endif //LINK_C_SDK_ROUTER_H
