#ifndef _LED_UART_H_
#define _LED_UART_H_

#include "bsp.h"

void LED_UART_Init(void);
void USART1WriteBuffer(const uint8_t *pBuffer,uint32_t NumByteToWrite);

#endif /* _LED_UART_H_ */
