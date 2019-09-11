#include "bsp.h"

//中断缓存串口数据

uint16_t uart_p;//接收数据计数
uint8_t uart_buff[UART_BUFF_SIZE];//定义接收数据Buff
uint8_t uart_buffValid;//1表示有数据需要处理,0表示没有数据

static void UartVarInit(void);

static void UartVarInit(void){
	uart_p = 0;
	uart_buffValid = 0;//清除标记
	memset(uart_buff, 0, sizeof(uart_buff));//清除buff
}


/// 配置USART接收中断
static void GPRS_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = GPRS_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART2_NVIC_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：GPRS_USART_Init
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void GPRS_USART_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config GPRS_USARTx clock */
	RCC_APB2PeriphClockCmd(GPRS_USART_GPIO_CLK, ENABLE);
	GPRS_USART_APBxClkCmd(GPRS_USART_CLK, ENABLE);

	/* USART GPIO config */
	/* Configure USART Tx  as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPRS_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPRS_TX_GPIO_PORT, &GPIO_InitStructure);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPRS_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPRS_RX_GPIO_PORT, &GPIO_InitStructure);

	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = GPRS_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(GPRS_USARTx, &USART_InitStructure); 

	/*	配置中断优先级 */
	GPRS_NVIC_Config();
	
	USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, DISABLE);//空闲中断
	/* 使能串口2接收中断 */
	USART_ITConfig(GPRS_USARTx, USART_IT_RXNE, ENABLE);//接收中断


	USART_Cmd(GPRS_USARTx, ENABLE);
	
	UartVarInit();
}


/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被GPRS_USARTx_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

void BSP_GPRS_USART_IRQHandler(void)
{
		if(Transmission_Sta)                                   //指令状态
	{
		if(USART2->DR)                                     //非零值才保存
		{
			SIM800C_RX_BUF[SIM800C_RX_STA]=USART2->DR;     //读取数据	
			SIM800C_RX_STA++;                              //SIM800C_RX_STA计数，收到多少数据
		}
	}
	else                                            //数据透传状态
	{	  
		if(SIM800C_RX_STA==0)		                    //收到第一个数据     
			TIM_Cmd(TIM3, ENABLE);                  //开启定时器3
		SIM800C_RX_BUF[SIM800C_RX_STA]=USART2->DR;        //读取数据	
		TIM_SetCounter(TIM3, 0);                    //清零定时器3计数器			    		
		SIM800C_RX_STA++;                              //SIM800C_RX_STA计数，收到多少数据 
	}  
}

//获取接收到的数据和长度
char *get_rebuff(uint16_t *len)
{
    *len = uart_p;
    return (char *)&uart_buff;
}

void clean_rebuff(void)
{
	uint16_t i = UART_BUFF_SIZE+1;
    uart_p = 0;
	while(i)
		uart_buff[--i]=0;
}

/*
 * 函数名：GPRS_USARTx_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即GPRS_USARTx
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用GPRS_USARTx_printf( GPRS_USARTx, "\r\n this is a demo \r\n" );
 *            		 GPRS_USARTx_printf( GPRS_USARTx, "\r\n %d \r\n", i );
 *            		 GPRS_USARTx_printf( GPRS_USARTx, "\r\n %s \r\n", j );
 */
void GPRS_USART_printf(char *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(GPRS_USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(GPRS_USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(GPRS_USARTx,*s);
						while( USART_GetFlagStatus(GPRS_USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(GPRS_USARTx,*s);
						while( USART_GetFlagStatus(GPRS_USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(GPRS_USARTx, *Data++);
		while( USART_GetFlagStatus(GPRS_USARTx, USART_FLAG_TXE) == RESET );
	}
}
