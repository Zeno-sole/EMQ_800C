#ifndef __GPRS_H
#define __GPRS_H

#include "bsp.h"

typedef enum{
    GPRS_TRUE,
    GPRS_FALSE,
}gprs_res_e;

#define     GPRS_CLEAN_RX()              clean_rebuff()
#define     GPRS_TX(cmd)                 GPRS_USART_printf("%s",cmd)
#define     GPRS_RX(len)                 ((char *)get_rebuff(&(len)))
#define     GPRS_DELAY(time)             BSP_OS_TimeDly(time) //延时
#define     GPRS_SWAP16(data)    				 __REVSH(data)

uint8_t Send_AT_Command(char *cmd, char *reply, uint32_t waittime);

uint8_t Check_ACK(char *reply);

void GPRS_Connect(void);//GPRS联网

uint8_t GPRS_Init(void);//初始化并检测模块

uint8_t IsInsertCard(void);//检测SIM卡是否插入

uint8_t GSM_GPRS_init(void);//GPRS初始化环境

uint8_t GPRS_TCPLink(char *localport, char * serverip, char * serverport);//TCP连接

uint8_t GPRS_UDPLink(char *localport, char * serverip, char * serverport);//UDP连接

uint8_t GPRS_Send(const char * str);//发送数据

uint8_t GPRS_LinkClose(void);//IP链接断开
uint8_t GPRS_ShutClose(void);//关闭场景

void GPRS_PWR(uint16_t sec);

void GPRS_Poll(void);//轮询接收GPRS数据
void HeartPack(void);//检测心跳包
void UploadData(void);//上传传感器数据
#endif
