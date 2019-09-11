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
#include "bsp_os.h" //�����ṩ�弶��ʱ����

#include "bsp_int.h" //�жϳ�ʼ��,�ṩϵͳʱ��
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
#define FLASH_PAGE_SIZE                      ((uint16_t)0x400) //����ҳ��СΪ1024Byte
#define FLASH_BASE_ADDR     	               ((uint32_t)0x08000000)
#define WORD_WIDE                             2  //��д���ݿ��16λ
#define FLASH_START_ADDR                     ((uint32_t)0x0803FC00)	//STM32F103VCT6��ַ��Χ0x08000000~0x0803FFFF = 256K, �������1024�ֽ�����EEPROM�洢
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
	uint16_t PRESS;            //����ѹ	
	uint16_t GPRS_FLAG;
	uint8_t WIND_DIRECTION_CLASS;	
};

extern struct SensorDataType mySensorDatas;//ȫ������
extern uint8_t modeSeq[5];//ִ����ʾ����

extern OS_TCB				   AppTaskStartTCB;
extern OS_TCB          AppTaskCollectionTCB;
extern OS_TCB          AppTaskSendTCB;
extern OS_TCB          AppTaskGET1TCB;
/*-------------------------------------------------------------------------*/
//extern uint16_t DEVICE_ID;
extern uint8_t Mode;
/*-------------------- Config���ڽ������ݱ���-------------------*/
extern uint8_t  RxDataBuff[256];  //�������ݽ��ջ�����
extern uint8_t  RxCmdBuff[128];   //����������ջ�����
extern uint16_t RxDataCount;      //�������ݼ�����
extern uint16_t RxCmdCount;       //�������������
extern uint8_t  RxStateConfig;    //������һ֡����
extern uint8_t  CmdFlag;          //0��ʾ����,1��ʾ����
/*-------------------- PM2.5/PM10/��ʪ��/����/���ڽ������ݱ���-------------------*/
extern uint8_t PM25_RxBuff[256];
extern uint16_t PM25_RxBuffCount;
/*-------------------------- ȫ�ֺ������� ------------------------------------*/
void BSP_Init(void);
void Soft_Reset(void);

void FLASH_ReadHalfWord(uint8_t *pBuffer, uint16_t len);
void Flash_WriteHalfWord(const uint8_t *pBuffer, uint16_t len);
void delay_ms(u16 num);
/*----------------------------------------------------------------------------*/
#endif  /*__BSP_H*/
