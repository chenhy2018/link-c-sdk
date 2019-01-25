//
// Created by chenh on 2019/1/24.
//

#include "device.h"

#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_KEEPALIVE 15
#define UNIT_LEN 256

struct DeviceObj gDevObj = {};

struct DevObj *GetDeviceObj();




struct UsageTab {
    char opt;
    char *pDescr;
};

static void DevShowUsage()
{
    struct UsageTab usageTab[] = {
        {7, ""},
        {'f', "printf this usage"},
        {'p', "device's dak"},
        {'u', "device's dsk"},
        {'h', "mqtt host to connect to"},
        {'P', "mqtt port to connect on"},
        {'b', "keep alive second"},
        {'r', "mqtt role, p:pub  other sub"}
    };

    int row = (int)usageTab[0].opt;
    int i;
    for (i = 1; i < row; i++) {
        DevPrint(DEV_LOG_ERR, "-%c: %s\n", usageTab[i].opt, usageTab[i].pDescr);
    }
}

static void ParseArgs(int argc, char *argv[])
{
    int rc = DEV_CODE_SUCCESS;
    char ch;

    if (argc < 2) {
        DevicePrint(3, "Device app options usage\n");
        DevShowUsage();
        return;
    }

    while ((ch = getopt(argc, argv, "f:p:u:h:P:b:r:")) != -1) {
        switch (ch) {
            case 'f':
                gDevObj.stDevcfg.conf = (char *)DevMalloc(strlen(optarg) +1);
                memcpy(gDevObj.stDevcfg.conf, optarg, strlen(optarg));
                break;
            case 'p':
                gDevObj.sDak = (char *)DevMalloc(strlen(optarg) + 1);
                memcpy(gDevObj.sDak, optarg, strlen(optarg));
                break;
            case 'u':
                gDevObj.sDsk = (char *)DevMalloc(strlen(optarg) + 1);
                memcpy(gDevObj.sDsk, optarg, strlen(optarg));
                break;
            case 'h':
                gDevObj.stDevcfg.stMqttcfg.sServer = (char *)DevMalloc(strlen(optarg) +1);
                memcpy(gDevObj.stDevcfg.stMqttcfg.sServer, optarg, strlen(optarg));
                break;
            case 'P':
                gDevObj.stDevcfg.stMqttcfg.nPort = atoi(optarg);
                break;
            case 'b':
                gDevObj.stDevcfg.stMqttcfg.nKeepAlive = atoi(optarg);
                break;
            case 'r':
                DevicePrint(DEV_LOG_ERR, "-r : %s\n", optarg);
                gDevObj.stDevcfg.stMqttcfg.enRole =
                    optarg[0] == 'p' ? MQTT_ROLE_PUB : MQTT_ROLE_SUB;
                break;
            default :
                return;
        }
    }
}

static void GenerateUserName(char *username, int *len, char *dak)
{
    long timestamp = 0.0;

    timestamp = (long)time(NULL);
    sprintf(username, "dak=%s&timestamp=%ld&version=v1", dak, timestamp);
    *len = strlen(username);
}

static int GeneratePassword(char *username, int unlen, char *password, int *passlen, char *dak)
{
    int ret = 0;
    int sha1len = 20;
    char sha1[UNIT_LEN] = {};

    ret = HmacSha1(dsk, strlen(dsk), username, unlen, sha1, &sha1len);
    if (ret != DEV_CODE_SUCCESS) {
        DevPrint(DEV_LOG_ERR, "Hmacsha1 failed.\n");
        return DEV_CODE_ERROR;
    }

    int len = urlsafe_b64_encode(sha1, 20, password, passlen);
    *passlen = len;
    return DEV_CODE_SUCCESS;
}

struct DeviceObj *NewDeviceObj()
{
    return &gDevObj;
}

struct deviceObj *GetDevObj()
{
    return &gDevObj;
}

void DelDeviceObj(struct DeviceObj *devObj)
{
    if (devObj->sDak)
        DevFree(devObj->sDak);
    if (devObj->sDsk)
        DevFree(devObj->sDsk);
    if (devObj->stDevcfg.conf)
        DevFree(devObj->stDevcfg.conf);
    if (devObj->stDevcfg.stMqttcfg.sServer)
        DevFree(devObj->stDevcfg.stMqttcfg.sServer);
    if (devObj->stDevcfg.stMqttcfg.sId)
        DevFree(devObj->stDevcfg.stMqttcfg.sId);
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

int main(int argc, char *argv[])
{
    ParseArgs(argc, argv);

    struct MqttOptions *mqttOpt = NULL;
    struct DeviceObj *devObj = NewDeviceObj();

}