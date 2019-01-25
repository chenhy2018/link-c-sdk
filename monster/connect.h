#ifndef LINK_C_SDK_CONNECT_H
#define LINK_C_SDK_CONNECT_H


struct ConnectStatus {
    void *pInstance;
    int nStatus;
    int nCount;
};

enum ConnStatus {
    CONNECT_INVALID = 0,
    CONNECT_OPEN,
    CONNECT_START,
    CONNECT_STOP,
    CONNECT_CLOSE
};

struct ConnectOperations {
    int (*Open)(struct ConnectObj *obj);
    void (*Close)(struct ConnectObj *obj);
    void (*RecvMessage)(struct ConnectObj *obj);
    void (*RecvEvent)(struct ConnectObj *obj);
    void (*SendMessage)(struct ConnectObj *obj);
};

struct ConnectObj {
    struct MqttInstance *stInstance;   //mqtt实例
    struct ConnectOperations *stOpt;
//    enum ConnStatus enStatus;
    int nStatus;

    char *sDak;
    char *sDsk;
    char *sServer;
    int nPort;
    int nKeepAlive;
    char *sId;
};

void ConnectMqtt();
void DisconnectMqtt();

#endif //LINK_C_SDK_CONNECT_H
