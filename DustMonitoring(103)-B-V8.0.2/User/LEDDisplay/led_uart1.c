#include "bsp.h"

static void USART_Configuration(void);

void LED_UART_Init(void){
	USART_Configuration();
}

static void USART_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);/*Enable USART1 clock */

	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  /* USART1 configured as follow:
        - BaudRate = 57600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 57600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;//仅发送
	/* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);
	
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//不使用接收中断
	
  /* Enable the USART1*/
  USART_Cmd(USART1, ENABLE);
	
	USART_ClearFlag(USART1,USART_FLAG_TC);
}

void USART1WriteBuffer(const uint8_t *pBuffer,uint32_t NumByteToWrite)
{
	while(NumByteToWrite--)
	{
		USART_SendData(USART1,(uint8_t)*pBuffer++);
		//printf("%02X \r\n", USART1->DR);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		
	}
	//printf("\r\n");
}
