//
// Created by chenh on 2019/1/23.
//

#ifndef LINK_C_SDK_COMMON_H
#define LINK_C_SDK_COMMON_H

#define __compiler_offsetof(a,b) __builtin_offsetof(a,b)
#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE,MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({\
    const typeof(((type*)0)->member) *__mptr = (ptr);\
    (type *)((char *)__mptr - offsetof(type,member));})

#define ARGSBUF_LEN 256
#define DEV_LOG_ERR 1
#define DEV_LOG_WARN 2
#define DEV_LOG_DBG 4


void DevPrint(int level, const char *fmt, ...);
void *DevMalloc(int size);
void DevFree(void *ptr);
int HmacSha1(const char *pKey, int nKeyLen, const char  *pInput,
	     int nInputLen, char *pOutput, char *dsk);

#endif //LINK_C_SDK_COMMON_H
