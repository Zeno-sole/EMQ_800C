#ifndef __BSP_H
#define __BSP_H

#include "stm32f10x.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
/*--------------------------------- uCOS-III -------------------------------------------*/
#include "os.h"
#include "cpu.h"
#include "app_cfg.h"
#include "os_app_hooks.h"
#include "cpu_core.h"
#include "bsp_os.h" //里面提供板级延时函数

#include "bsp_int.h" //中断初始化,提供系统时钟
/* ----------------------- Modbus includes --------------------------------*/
#include "modbus_master.h"
#include "modbus_uart.h"
#include "trans_recieve_buff_control.h"
/* ------------------------- BSP includes ---------------------------------*/
#include "bsp_debug.h"
#include "bsp_gpio.h"
#include "bsp_config_usart.h"
/*-------------------------------------------------------------------------*/
#include "noise_sensor.h"
#include "wind_sensor.h"
#include "pm25_uart3.h"
/*-------------------------------------------------------------------------*/
#include "gprs_usart.h"
#include "gprs.h"
#include "led_display.h"
#include "led_uart1.h"

#include "opt.h"
/*-------------------------------EMQ---------------------------------------*/
#include "BSP_EMQ.h"
/*-------------------------------------------------------------------------*/
#define USART2_NVIC_PRIORITY                      0
#define USART3_NVIC_PRIORITY                      1
#define UART4_NVIC_PRIORITY                       2
#define UART5_NVIC_PRIORITY                       3
/*-------------------------------------------------------------------------*/
#define FLASH_PAGE_SIZE                      ((uint16_t)0x400) //定义页大小为1024Byte
#define FLASH_BASE_ADDR     	               ((uint32_t)0x08000000)
#define WORD_WIDE                             2  //读写数据宽度16位
#define FLASH_START_ADDR                     ((uint32_t)0x0803FC00)	//STM32F103VCT6地址范围0x08000000~0x0803FFFF = 256K, 保留最后1024字节用于EEPROM存储
/*-------------------------------------------------------------------------*/
#define NOISE_SENSOR_ADDR                        0x01
#define WIND_SPEED_SENSOR_ADDR                   0x03
#define WIND_DIRECTION_SENSOR_ADDR               0x04
/*-------------------------------------------------------------------------*/
#define READ_RS485_COUNTS        2
/*-------------------------------------------------------------------------*/
struct SensorDataType {
	uint16_t PM2_5;
	uint16_t PM10;
	uint16_t TSP;
	uint16_t NOISE;
	uint16_t TEMP;
	uint16_t HUMI;
	uint16_t WIND_SPEED;
	uint16_t WIND_DIRECTION;
	uint16_t WIND_LEVEL;
	uint16_t PRESS;            //大气压	
	uint16_t GPRS_FLAG;
	uint8_t WIND_DIRECTION_CLASS;	
};

extern struct SensorDataType mySensorDatas;//全局数据
extern uint8_t modeSeq[5];//执行显示序列

extern OS_TCB				   AppTaskStartTCB;
extern OS_TCB          AppTaskCollectionTCB;
extern OS_TCB          AppTaskSendTCB;
extern OS_TCB          AppTaskGET1TCB;
/*-------------------------------------------------------------------------*/
//extern uint16_t DEVICE_ID;
extern uint8_t Mode;
/*-------------------- Config串口接收数据变量-------------------*/
extern uint8_t  RxDataBuff[256];  //配置数据接收缓存区
extern uint8_t  RxCmdBuff[128];   //配置命令接收缓存区
extern uint16_t RxDataCount;      //接收数据计数器
extern uint16_t RxCmdCount;       //接收命令计数器
extern uint8_t  RxStateConfig;    //接收完一帧数据
extern uint8_t  CmdFlag;          //0表示命令,1表示数据
/*-------------------- PM2.5/PM10/温湿度/噪声/串口接收数据变量-------------------*/
extern uint8_t PM25_RxBuff[256];
extern uint16_t PM25_RxBuffCount;
/*-------------------------- 全局函数声明 ------------------------------------*/
void BSP_Init(void);
void Soft_Reset(void);

void FLASH_ReadHalfWord(uint8_t *pBuffer, uint16_t len);
void Flash_WriteHalfWord(const uint8_t *pBuffer, uint16_t len);
void delay_ms(u16 num);
/*----------------------------------------------------------------------------*/
#endif  /*__BSP_H*/
