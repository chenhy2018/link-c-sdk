#include "mqtt.h"
#include "mqtt_internal.h"
#include "connect.h"
#include "common.h"
#include "device.h"

#define CONN_NUM (2)
struct ConnectStatus Status[CONN_NUM];

#define CONNECT_CODE_SUCCESS (0)
#define CONNECT_CODE_FAIL (-1)

struct ConnectObj gConnObj = {};

void OnMessage(IN const void* _pInstance, IN int _nAccountId, IN const char* _pTopic, IN const char* _pMessage, IN size_t nLength)
{
    fprintf(stderr, "%p topic %s message %s \n", _pInstance _pTopic, _pMessage);
}

void OnEvent(IN const void* _pInstance, IN int _nAccountId, IN int _nId,  IN const char* _pReason)
{
    fprintf(stderr, "%p id %d, reason  %s \n",_pInstance, _nId, _pReason);
    struct ConnectStatus* pStatus;
    struct MqttInstance *instance = NULL;
    int i;

    instance = (struct MqttInstance *)_pInstance;
    for (i = 0; i < CONN_NUM; i++) {
        if (Status[i].pInstance == _pInstance) {
            pStatus = &Status[i];
            pStatus->nStatus = _nId;
            if (!strcmp(_pReason, "STATUS_CONNECT_TIMEOUT"
                 || !strcmp(_pReason, "Error (Network)"))) {
                int unlen = 0;
                int passlen = 0;
                struct DeviceObj *devObj = GetDeviceObj();
                GetUserName(instance->options.userInfo.pUsername, &unlen, devObj->sDak);
                GetSign(istance->options.userInfo.pUsername, unlen, instance->options.pPassword, &passlen, devObj->sDsk);
            }
        }
    }
}

struct ConnectStatus *GetLogInstance(IN int _nIndex)
{
    return &Status[_nIndex];
}

static struct MqttOptions *NewMqttOptions(const char *dak, const char *dsk, struct Mqttconfig cfg)
{
    struct MqttOptions *mqttOpt = DevMalloc(sizeof(struct MqttOptions));

    mqttOpt->bCleanSession = false;
    mqttOpt->userInfo.nAuthenicationMode = MQTT_AUTHENTICATION_USER;
    mqttOpt->userInfo.pHostname = cfg.sServer;
    mqttOpt->userInfo.nPort = cfg.nPort <= 0 ? DEFAULT_MQTT_PORT : cfg.nPort;
    mqttOpt->userInfo.pCafile = NULL;
    mqttOpt->userInfo.pCertfile = NULL;
    mqttOpt->userInfo.pKeyfile = NULL;
    mqttOpt->nKeepalive = cfg.nKeepAlive <= 0 ? DEFAULT_MQTT_KEEPALIVE : cfg.nKeepAlive;
    mqttOpt->nQos = 0;
    mqttOpt->bRetain = false;
    mqttOpt->callbacks.OnMessage = &OnMessage;
    mqttOpt->callbacks.OnEvent  =  &OnEvent;
    mqttOpt->pId = (char *)DevMalloc(strlen(dak)+1);
    memcpy(mqttOpt->pId, dak, strlen(dak));

    int unlen = 0;
    int passlen = 0;
    char username[UNIT_LEN] = {};
    char pass[UNIT_LEN] = {};
    GenerateUserName(username, unlen, dak);
    GeneratePassword(username, unlen, pass, passlen, &passlen, dsk);
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
    struct DeviceObj *devObj = container_of(obj, struct DeviceObj, stConnObj);

    char *dak = devObj->sDak;
    char *dsk = devObj->sDsk;
    struct MqttOptions *mqttOptions = NewMqttOptions(dak, dsk, devObj->stDevcfg.stMqttcfg);
    if (mqttOptions) {
        LinkMqttLibInit();
        void *connInstance = LinkMqttCreateInstance(mqttOptions);
        if (connInstance) {
            obj->stConnObj.stInstance = connInstance;
        }
    }
}

void ConnClose(struct ConnectObj *obj)
{

    LinkMqttDestroy(Status[0].pInstance);
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

struct ConnectObj *NewConnectObj()
{
    int ret = CONNECT_CODE_FAIL;

    gConnObj.stOpt = &gConnOpt;
    if (gConnObj.stOpt->Open) {
        ret = gConnObj.stOpt->Open(obj);   //建立mqtt底层连接
        gConnObj.enStatus = CONNECT_OPEN;
    }
    else {
        obj->enStatus = CONNECT_INVALID;
    }

    return &gConnObj;
}

void DelConnectObj(struct ConnectObj *obj)
{
    DevPrint(DEV_LOG_WARN, "DelConnectObj %p\n", obj);
}