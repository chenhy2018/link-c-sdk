#ifndef __LINK_BASE_H__
#define __LINK_BASE_H__

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <errno.h>
#include "log.h"
#ifndef __APPLE__
#include <stdint.h>
#endif

#ifndef OUT
#define OUT
#endif
#ifndef IN
#define IN
#endif

typedef enum _LinkUploadZone{
        LINK_ZONE_HUADONG = 1,
        LINK_ZONE_HUABEI = 2,
        LINK_ZONE_HUANAN = 3,
        LINK_ZONE_BEIMEI = 4,
        LINK_ZONE_DONGNANYA = 5,
}LinkUploadZone;

typedef struct _LinkUserUploadArg{
        char  *pToken_;
        int   nTokenLen_;
        LinkUploadZone uploadZone_;
        char  *pDeviceId_;
        int   nDeviceIdLen_;
        int   nUploaderBufferSize;
        int   nNewSegmentInterval;
}LinkUserUploadArg;

typedef enum {
        LINK_VIDEO_H264 = 1,
        LINK_VIDEO_H265 = 2
}LinkVideoFormat;

typedef enum {
        LINK_AUDIO_PCMU = 1,
        LINK_AUDIO_PCMA = 2,
        LINK_AUDIO_AAC = 3
}LinkAudioFormat;

typedef enum {
        LINK_UPLOAD_INIT,
        LINK_UPLOAD_FAIL,
        LINK_UPLOAD_OK
}LinkUploadState;

typedef struct _LinkMediaArg{
        LinkAudioFormat nAudioFormat;
        int nChannels;
        int nSamplerate;
        LinkVideoFormat nVideoFormat;
} LinkMediaArg;

#define LINK_STREAM_UPLOAD 1

#define LINK_NO_MEMORY       -1000
#define LINK_MUTEX_ERROR     -1100
#define LINK_COND_ERROR      -1101
#define LINK_THREAD_ERROR    -1102
#define LINK_TIMEOUT         -2000
#define LINK_NO_PUSH         -2001
#define LINK_BUFFER_IS_SMALL -2003
#define LINK_ARG_TOO_LONG    -2004
#define LINK_ARG_ERROR       -2100
#define LINK_JSON_FORMAT     -2200
#define LINK_HTTP_TIME       -2300
#define LINK_OPEN_TS_ERR     -2400
#define LINK_WRITE_TS_ERR    -2401
#define LINK_Q_OVERWRIT      -5001
#define LINK_Q_WRONGSTATE    -5002
#define LINK_SUCCESS         0


int LinkIsProcStatusQuit();

#endif
