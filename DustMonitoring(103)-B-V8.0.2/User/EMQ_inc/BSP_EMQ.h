#ifndef __BSO_EMQ_H
#define __BSO_EMQ_H	 
#include "bsp.h"
#include "stm32f10x.h"
//#include "delay.h"
//#include "usart.h"
//#include "led.h"
//#include "includes.h"
#include "timer1.h"
#include "timer2.h"
#include "timer3.h"
#include "timer4.h"
#include "timer5.h"
#include "timer6.h"
#include "sim800c.h"
#include "sim800c_usart.h"
#include "emqtt.h"
//#include "includes.h"
#include "stmflash.h"
#define FLASH_DEVICE_ID_ADDR           			 0X08020000




extern u8 Transmission_Sta;
extern u8 serverLink_sta;
extern u8 ping_flag;
extern u8 Link_restart;
extern u8 EMQ_RES;
extern u8 EMQ_success;
extern char *Cntr_CMD ;    //控制端的topic名称
extern char *Deal_CMD ;    //执行端的topic名称
extern uint8_t  RxCmdBuff[128];   //配置命令接收缓存区
extern uint16_t RxCmdCount;      //接收数据计数器
extern uint32_t DEVICE_ID;
extern  uint32_t falsh_DEVICE_ID[2];	//write flash 缓存数组
extern  uint32_t falsh_DEVICE_id[2];
void UartWriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite);	
void EMQ_Init(void);
void EMQ_POLL(void);
void ParseCommand_Config(void);
void write_DEVICE_ID(void);
void read_DEVICE_ID(void);
void RELAY_POLL(void);
void delay_s(u8 num);

void IWDG_Feed(void);
void IWDG_Config(uint8_t prv ,uint16_t rlv);
void EMQ_Connect(void);
void ResConnect(void);
void Tcp_Connect(void);
#endif
