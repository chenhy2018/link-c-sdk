
#include "segmentmgr.h"
#include "servertime.h"
#include <string.h>
#include <time.h>
#include "tsuploaderapi.h"

static char segStoreToken[1024];
static int segGetTokenCallback(void *pOpaque, char *pBuf, int nBuflen) {
        LinkLogDebug("in segGetTokenCallback");
        memcpy(pBuf, segStoreToken, strlen(segStoreToken));
        return strlen(segStoreToken);
}

void JustTestSegmentMgr(const char *pUpToken, const char *pMgrUrl) {
        int ret = LinkInitTime();
        assert(ret == LINK_SUCCESS);
        ret = LinkGetUploadToken(segStoreToken, sizeof(segStoreToken), NULL, pUpToken);
        if (ret != LINK_SUCCESS) {
                LinkLogError("LinkGetUploadToken fail:%d", ret);
                return;
        }
        LinkLogDebug("jseg token:%s\n", segStoreToken);
        
        ret = LinkInitSegmentMgr();
        if (ret != LINK_SUCCESS) {
                LinkLogError("LinkInitSegmentMgr fail:%d", ret);
                return;
        }
        
        SegmentHandle segHandle;
        SegmentArg arg;
        arg.getTokenCallback = segGetTokenCallback;
        arg.pGetTokenCallbackArg = NULL;
        arg.pDeviceId = "abc";
        arg.nDeviceIdLen = 3;
        arg.pMgrTokenRequestUrl = (char *)pMgrUrl;
        arg.nMgrTokenRequestUrlLen = strlen(arg.pMgrTokenRequestUrl);
        arg.useHttps = 0;
        arg.pUploadStatisticCb = NULL;
        arg.pUploadStatArg = NULL;
        ret = LinkNewSegmentHandle(&segHandle, &arg);
        if (ret != LINK_SUCCESS) {
                LinkLogError("LinkInitSegmentMgr fail:%d", ret);
                return;
        }
        
        int count = 5;
        struct timespec tp;
        
        clock_gettime(CLOCK_MONOTONIC, &tp);
        int64_t nStart = tp.tv_sec * 1000;
        int64_t nEnd = nStart+5320;
        while(count) {
                LinkUpdateSegment(segHandle, nStart, nEnd, 0);
                nEnd+=5222;
                count--;
                sleep(5);
        }
}
