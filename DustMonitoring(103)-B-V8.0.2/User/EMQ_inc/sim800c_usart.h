/*-------------------------------------------------*/

/*-------------------------------------------------*/

#ifndef __SIM800C_USART_H
#define __SIM800C_USART_H

#include "stdio.h"
#include "stdarg.h"		 
#include "string.h"
#include "BSP_EMQ.h" 

#define SIM800C_RXBUFF_SIZE      128                  //������ջ�������С256�ֽ�
#define SIM800C_TXBUFF_SIZE      2048                  //������ջ�������С256�ֽ�

extern  u8  SIM800C_RX_BUF[SIM800C_RXBUFF_SIZE];      //���ջ������� 
extern  u16 SIM800C_RX_STA;         		          //����״̬���	

extern  u8  SIM800C_TX_BUF[SIM800C_TXBUFF_SIZE];      //���ͻ������� 

void   SIM800CUsart_init(u32 bound);               //��ʼ������
void   SIM800C_printf(char* fmt,...) ;             //printf����
void   SIM800C_TxData(u8 *data);
void SIM800C_TxChar(u8 data);

#endif


