#include "mqtt.h"
#include "control.h"
#include "control_internal.h"
#include "connect.h"
#include "common.h"

#define CONN_NUM (2)
struct ConnectStatus Status[CONN_NUM];


#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_KEEPALIVE 15
#define DEFAULT_MQTT_PID "mqttPub"


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

    if (gConnObj.stInstance == _pInstance) {
        gConnObj.nStatus = _nId;
        if (!strcmp(_pReason, "STATUC_CONNECT_TIMEOUT")
            || !strcmp(_pReason, "Error (Network)")) {
            int unlen = 0;
            int passlen = 0;
            //GenerateUserName(instance->options.userInfo.pUsername, &unlen, gConnObj.sDak);
            //GeneratePassword(instance->options.userInfo.pUsername, unlen,
//                             instance->options.userInfo.pPassword, &passlen, gConnObj.sDsk);
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
//    mqttOpt->pId = (char *)DevMalloc(strlen(dak)+1);
//    memcpy(mqttOpt->pId, dak, strlen(dak));
    mqttOpt->pId = (char *)DevMalloc(strlen(DEFAULT_MQTT_PID) + 1);
    memcpy(mqttOpt->pId, DEFAULT_MQTT_PID, sizeof(DEFAULT_MQTT_PID));

    int unlen = 0;
    char username[256] = {};
    int passlen = 0;
    char pass[128] = {};

//    GenerateUserName(username, &unlen, dak);
//    GeneratePassword(username, unlen, pass, &passlen, dsk);
//    GetUserName(username, &unlen, dak);
//    GetSign(username, unlen, pass, &passlen, dsk);

//    DevPrint(DEV_LOG_ERR, "username :%s  pass:%s unlen:%d  passlen:%d\n", username, pass, unlen, passlen);

#define USERNAME "1008"
#define PASSWORD "m36SCkF6"
    mqttOpt->userInfo.pUsername = (char *)DevMalloc(sizeof(USERNAME));
    memset(mqttOpt->userInfo.pUsername, 0, sizeof(USERNAME));
    memcpy(mqttOpt->userInfo.pUsername, USERNAME, strlen(USERNAME));
    mqttOpt->userInfo.pPassword = (char *)DevMalloc(sizeof(PASSWORD));
    memset(mqttOpt->userInfo.pPassword, 0, sizeof(PASSWORD));
    memcpy(mqttOpt->userInfo.pPassword, PASSWORD, strlen(PASSWORD));

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

static void ShowMqttOptiuons(struct MqttOptions *obj)
{
    printf("bCleanSession:%s\n", obj->bCleanSession == false ? "false" : "true");
    printf("userInfo.nAuthenicatinMode:%s\n", obj->userInfo.nAuthenicatinMode == MQTT_AUTHENTICATION_USER ? "MQTT_AUTHENTICATION_USER" : "OTHER");
    printf("userInfo.pHostname:%s\n", obj->userInfo.pHostname);
    printf("userInfo.nPort:%d\n", obj->userInfo.nPort);
    printf("nKeepalive:%d\n", obj->nKeepalive);
    printf("nQos:%d\n", obj->nQos);
    printf("bRetain:%s\n", obj->bRetain == false ? "false" : "true");
    printf("pId:%s\n", obj->pId);
    printf("userInfo.pUsername:%s\n", obj->userInfo.pUsername);
    printf("userInfo.pPassword:%s\n", obj->userInfo.pPassword);
}
int ConnOpen(struct ConnectObj *obj)
{
    struct MqttConfig mqttCfg = {};

    char *dak = obj->sDak;
    char *dsk = obj->sDsk;
    mqttCfg.sServer = obj->sServer;
    mqttCfg.nPort = obj->nPort;
    mqttCfg.nKeepAlive = obj->nKeepAlive;
    mqttCfg.sId = obj->sId;

    printf("dak:%s   dsk:%s\n", dak, dsk);

    struct MqttOptions *mqttOptions = NewMqttOptions(dak, dsk, mqttCfg);


    if (mqttOptions) {
        LinkMqttLibInit();
        gConnObj.stInstance = LinkMqttCreateInstance(mqttOptions);
        while (gConnObj.nStatus != MQTT_SUCCESS) {
            sleep(1);
        }
        printf("connect succeed.\n");
        LinkMqttSubscribe(gConnObj.stInstance, "linking/v1/${appid}/${device}/rpc/request/+/");
        gConnObj.nSession = LinkInitIOCtrl("test", "ctrl001", gConnObj.stInstance);
        LinkInitLog("test", "ctrl001", gConnObj.stInstance);
    }
    DelMqttOptions(mqttOptions);
}

void ConnClose(struct ConnectObj *obj)
{
    LinkDinitIOCtrl(obj->nSession);
    LinkDinitLog();
    gConnObj.nStatus = 0;
    LinkMqttDestroy(gConnObj.stInstance);
    LinkMqttLibCleanup();
}

//void ConnRecvMessage(struct ConnectObj *obj)
//{
//
//}
//
void ConnSendMessage(struct ConnectObj *obj)
{
    LinkSendIOResponse(obj->nSession, 0, "ctrl", 3);
    LinkSendLog(5, "ctrl0013333testest", 13);
}

struct ConnectOperations gConnOpt = {
    .Open = ConnOpen,
    .Close = ConnClose,
//    .RecvMessage = ConnRecvMessage,
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
    gConnObj.stOpt = &gConnOpt;
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

static void AssignThread(void *pData)
{
    struct ConnectObj *connObj = (struct ConnectObj *)pData;
    int ret = DEV_CODE_SUCCESS;
    int cmdParamMax = 256;
    char *cmdParam = (char *)DevMalloc(cmdParamMax);
    int cmd = 0;

    while (connObj->nSession) {
        ret = LinkRecvIOCtrl(connObj->nSession,  &cmd,  cmdParam, &cmdParamMax, 1000);
        if (ret == MQTT_RETRY) {
            usleep(1000);
        } else if (ret == MQTT_SUCCESS) {
            connObj->stRouter.stOpt->assign();
        }

   }
}