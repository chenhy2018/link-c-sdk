#include "mqtt.h"
//#include "mqtt_internal.h"
#include "connect.h"
#include "common.h"
//#include "device.h"

#define CONN_NUM (2)
struct ConnectStatus Status[CONN_NUM];


#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_KEEPALIVE 15

#define CONNECT_CODE_SUCCESS (0)
#define CONNECT_CODE_FAIL (-1)

struct MqttConfig {
    char *sServer;
    int nPort;
    int nKeepAlive;
    char *sId;
};

struct MqttInstance
{
    void *mosq;
    struct MqttOptions options;
    int status;
    int lastStatus;
    bool connected;
    bool isDestroying;
}MqttInstance;

struct ConnectObj gConnObj = {};

void MessageCallback(const void* _pInstance, IN int _nAccountId, IN const char* _pTopic, IN const char* _pMessage, IN size_t nLength)
{
    DevPrint(DEV_LOG_ERR, "%p topic %s message %s \n", _pInstance, _pTopic, _pMessage);
}

void EventCallback(const void* _pInstance, IN int _nAccountId, IN int _nId,  IN const char* _pReason)
{
    DevPrint(DEV_LOG_DBG, "%p id %d, reason  %s \n",_pInstance, _nId, _pReason);
    struct MqttInstance *instance = &gConnObj.stInstance;

//    instance = (struct MqttInstance *)_pInstance;
    if (gConnObj.stInstance == _pInstance) {
        gConnObj.nStatus = _nId;
        if (!strcmp(_pReason, "STATUC_CONNECT_TIMEOUT")
            || !strcmp(_pReason, "Error (Network)")) {
            int unlen = 0;
            int passlen = 0;
            GenerateUserName(instance->options.userInfo.pUsername, &unlen, gConnObj.sDak);
            GeneratePassword(instance->options.userInfo.pUsername, unlen,
                             instance->options.userInfo.pPassword, &passlen, gConnObj.sDsk);
        }
    }
}

static struct MqttOptions *NewMqttOptions(const char *dak, const char *dsk, struct MqttConfig cfg)
{
    struct MqttOptions *mqttOpt = DevMalloc(sizeof(struct MqttOptions));

    mqttOpt->bCleanSession = false;
    mqttOpt->userInfo.nAuthenicatinMode = MQTT_AUTHENTICATION_USER;
    mqttOpt->userInfo.pHostname = cfg.sServer;
    mqttOpt->userInfo.nPort = cfg.nPort <= 0 ? DEFAULT_MQTT_PORT : cfg.nPort;
    mqttOpt->userInfo.pCafile = NULL;
    mqttOpt->userInfo.pCertfile = NULL;
    mqttOpt->userInfo.pKeyfile = NULL;
    mqttOpt->nKeepalive = cfg.nKeepAlive <= 0 ? DEFAULT_MQTT_KEEPALIVE : cfg.nKeepAlive;
    mqttOpt->nQos = 0;
    mqttOpt->bRetain = false;
    mqttOpt->callbacks.OnMessage = &MessageCallback;
    mqttOpt->callbacks.OnEvent  =  &EventCallback;
    mqttOpt->pId = (char *)DevMalloc(strlen(dak)+1);
    memcpy(mqttOpt->pId, dak, strlen(dak));

    int unlen = 0;
    int passlen = 0;
    char username[UNIT_LEN] = {};
    char pass[UNIT_LEN] = {};
    GenerateUserName(username, unlen, dak);
    GeneratePassword(username, unlen, pass, &passlen, dsk);
    mqttOpt->userInfo.pUsername = (char *)DevMalloc(unlen + 1);
    memcpy(mqttOpt->userInfo.pUsername, username, unlen);
    mqttOpt->userInfo.pPassword = (char *)DevMalloc(passlen + 1);
    memcpy(mqttOpt->userInfo.pPassword, pass, passlen);
    return mqttOpt;
}

static void DelMqttOptions(struct MqttOptions *ptr)
{
    if (ptr->userInfo.pUsername)
        DevFree(ptr->userInfo.pUsername);
    if (ptr->userInfo.pPassword)
        DevFree(ptr->userInfo.pPassword);
    if (ptr->pId)
        DevFree(ptr->pId);
    DevFree(ptr);
}

int ConnOpen(struct ConnectObj *obj)
{
//    struct DeviceObj *devObj = container_of(obj, struct DeviceObj, stConnObj);
    struct MqttConfig mqttCfg = {};

    char *dak = obj->sDak;
    char *dsk = obj->sDsk;
    mqttCfg.sServer = obj->sServer;
    mqttCfg.nPort = obj->nPort;
    mqttCfg.nKeepAlive = obj->nKeepAlive;
    mqttCfg.sId = obj->sId;
    struct MqttOptions *mqttOptions = NewMqttOptions(dak, dsk, mqttCfg);
    if (mqttOptions) {
        LinkMqttLibInit();
        gConnObj.stInstance = LinkMqttCreateInstance(mqttOptions);
        if (gConnObj.nStatus != MQTT_SUCCESS) {
            sleep(1);
        }
    }
    DelMqttOptions(mqttOptions);
}

void ConnClose(struct ConnectObj *obj)
{
    gConnObj.nStatus = 0;
    LinkMqttDestroy(gConnObj.stInstance);
    LinkMqttLibCleanup();
}

void ConnRecvMessage(struct ConnectObj *obj)
{

}

void ConnSendMessage(struct ConnectObj *obj)
{

}

struct ConnectOperations gConnOpt = {
    .Open = ConnOpen,
    .Close = ConnClose,
    .RecvMessage = ConnRecvMessage,

    .SendMessage = ConnSendMessage
};

struct ConnectObj *NewConnectObj(char *dak, char *dsk, void *cfg)
{
    int ret = CONNECT_CODE_FAIL;
    struct MqttConfig *mqttCfg = (struct MqttConfig *)cfg;

    gConnObj.stOpt = &gConnOpt;
    gConnObj.sDak = dak;
    gConnObj.sDsk = dsk;
    gConnObj.sServer = mqttCfg->sServer;
    gConnObj.nPort = mqttCfg->nPort;
    gConnObj.nKeepAlive = mqttCfg->nKeepAlive;
    gConnObj.sId = mqttCfg->sId;
    if (gConnObj.stOpt->Open) {
        ret = gConnObj.stOpt->Open(&gConnObj);   //建立mqtt底层连接
        gConnObj.nStatus = CONNECT_OPEN;
    }
    else {
        gConnObj.nStatus = CONNECT_INVALID;
    }

    return &gConnObj;
}

void DelConnectObj(struct ConnectObj *obj)
{
    if (obj->stOpt->Close) {
        obj->stOpt->Close(obj);
    }
    DevPrint(DEV_LOG_WARN, "DelConnectObj %p\n", obj);
}