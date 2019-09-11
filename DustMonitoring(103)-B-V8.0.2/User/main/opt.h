#ifndef _COMMON_H_
#define _COMMON_H_

#include "bsp.h"


uint8_t GetString(char *tgt,uint16_t numofsize,const char *src,const char *str);//获取指示标签的字符串,获取成功返回1,否则返回0

void trim(char *s);
char * left(char *dst,char *src, int n);
char * mid(char *dst,char *src, int nLen,int nPos);
char * right(char *dst,char *src, int nLen);
int32_t StringFind(const char *pSrc, const char *pDst);
int htoi(char *s);

void CutString(char *pSrc, char *pDst, uint8_t maxSize);


#endif

