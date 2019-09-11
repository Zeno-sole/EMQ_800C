/*-------------------------------------------------*/

/*-------------------------------------------------*/

#ifndef __SIM800C_H
#define __SIM800C_H

#include "BSP_EMQ.h" 
//http://39.96.201.29
//http://39.98.210.195
//http://39.100.75.242   //永诚
#define SERVER_IP   "39.98.40.237"    //东方智达服务器
//#define SERVER_IP   "39.100.75.242"
#define SERVER_PORT  1883
//#define SERVER_IP   "183.230.40.39"
//#define SERVER_PORT  6002
#define  POWER_KEY    PDout(10)
#define  POWER_STA    PDin(9)

void SIM800C_init(void);
//u8 SIM800C_Power(void);
u8 SIM800C_TestAT(u16 timeout);
u8 SIM800C_SendCmd_OK(u8 *cmd, u16 timeout);
u8 SIM800C_CPIN(u16 timeout);
u8 SIM800C_CSQ(u16 timeout);
u8 SIM800C_QueryStatus(u16 timeout);
u8 SIM800C_ATD(char *num, u16 timeout);
u8 SIM800C_CMGS(char *num, u16 timeout);
u8 SIM800C_GPRS(u16 timeout);
u8 SIM800C_CREG(u16 timeout);
//int32_t StringFind(const char *pSrc, const char *pDst)  ;
#endif


