/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "BSP_EMQ.h"
#include "sim800c_usart.h"	

u8  SIM800C_RX_BUF[SIM800C_RXBUFF_SIZE];     //接收缓冲数组
u16 SIM800C_RX_STA=0;                       //接收状态标记	

/*-------------------------------------------------*/
/*函数名：初始化串口2                              */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SIM800CUsart_init(u32 bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;            //PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	     //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);               //初始化PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;            //PA3
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);               //初始化PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //收发模式
    USART_Init(USART2, &USART_InitStructure);                                      //按配置设置串口2
	
    USART_Cmd(USART2, ENABLE);                              //使能串口 2
	USART_ClearFlag(USART2, USART_FLAG_TC);	                //清除标志位
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);          //开启接受中断

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;       //串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//中断通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //根据配置设置
}

 __align(8) u8  SIM800C_TX_BUF[SIM800C_TXBUFF_SIZE];          //串口2发送区

/*-------------------------------------------------*/
/*函数名：串口2printf                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SIM800C_printf(char* fmt,...)  
{  
	u16 i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)SIM800C_TX_BUF,fmt,ap);
	va_end(ap);
	
	length=strlen((const char*)SIM800C_TX_BUF);	
	while((USART2->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = SIM800C_TX_BUF[i];
		while((USART2->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*函数名：串口2printf                              */
/*参  数：data：数据                               */
/*参  数：size：数据长度                           */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SIM800C_TxData(u8 *data)
{
	u8 i;
	
	while((USART2->SR&0X40)==0);
	for(i = 1;i <= data[0];i ++)
	{			
		USART2->DR = data[i];
		while((USART2->SR&0X40)==0);	
	}
}

/*-------------------------------------------------*/
/*函数名：串口2发送一个字节                        */
/*参  数：data：数据                               */
/*参  数：size：数据长度                           */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SIM800C_TxChar(u8 data)
{	
	while((USART2->SR&0X40)==0);
	USART2->DR = data;
}
