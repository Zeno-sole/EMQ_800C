/*-------------------------------------------------*/

/*-------------------------------------------------*/

#ifndef __SIM800C_USART_H
#define __SIM800C_USART_H

#include "stdio.h"
#include "stdarg.h"		 
#include "string.h"
#include "BSP_EMQ.h" 

#define SIM800C_RXBUFF_SIZE      128                  //定义接收缓冲区大小256字节
#define SIM800C_TXBUFF_SIZE      2048                  //定义接收缓冲区大小256字节

extern  u8  SIM800C_RX_BUF[SIM800C_RXBUFF_SIZE];      //接收缓冲数组 
extern  u16 SIM800C_RX_STA;         		          //接收状态标记	

extern  u8  SIM800C_TX_BUF[SIM800C_TXBUFF_SIZE];      //发送缓冲数组 

void   SIM800CUsart_init(u32 bound);               //初始化函数
void   SIM800C_printf(char* fmt,...) ;             //printf函数
void   SIM800C_TxData(u8 *data);
void SIM800C_TxChar(u8 data);

#endif


