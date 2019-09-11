#include "bsp.h"

static void ModbusUART_Config(void);

void InitModbusUART(void)
{
	ModbusUART_Config();
}

static void ModbusUART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	/*-------------------------------------------------------------------------------------------------------------------------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);/* Enable GPIO clock */
	/*-------------------------------------------------------------------------------------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);/*Enable UART4 clock */
	/*-------------------------------------------------------------------------------------------------------------------------*/
	/* Configure UART4 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure UART4 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* UART4 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);/* Configure UART4 */
	/* Configure UART4 */
  USART_Init(UART4, &USART_InitStructure);
	
  /* Enable UART4 Receive and Transmit interrupts */
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//接收中断
	USART_ITConfig(UART4, USART_IT_TC, ENABLE);//使用发送完成中断
  /* Enable the UART4*/
  USART_Cmd(UART4, ENABLE);
	
	USART_ClearFlag(UART4,USART_FLAG_TC);
	/*--------------------------------------------------------------------------------------*/
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the UART4 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART4_NVIC_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void ModbusUART_WriteBuffer(const uint8_t *pBuffer,uint32_t NumByteToWrite)
{
	while(NumByteToWrite--)
	{
		USART_SendData(UART4,(uint8_t)*pBuffer++);
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
	}
}
