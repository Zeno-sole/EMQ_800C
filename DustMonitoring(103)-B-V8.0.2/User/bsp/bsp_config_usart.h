#ifndef _BSP_USART_CONFIG_H_
#define _BSP_USART_CONFIG_H_

#include "bsp.h"

void BSP_InitUart_Config(void);
//void ParseCommand_Config(void);
//void UartWriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite);
void Uart5WriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite);
void Uart3WriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite);
u8 Find_usart5(char *a);    //查询接收缓存数组内容 
u8 Find_usart3(char *a);
void CLR_Buf5(void);
void CLR_Buf3(void);
#endif /* _BSP_USART_CONFIG_H_ */
