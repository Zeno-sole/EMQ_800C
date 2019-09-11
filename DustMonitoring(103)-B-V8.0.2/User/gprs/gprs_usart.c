#include "bsp.h"

//�жϻ��洮������

uint16_t uart_p;//�������ݼ���
uint8_t uart_buff[UART_BUFF_SIZE];//�����������Buff
uint8_t uart_buffValid;//1��ʾ��������Ҫ����,0��ʾû������

static void UartVarInit(void);

static void UartVarInit(void){
	uart_p = 0;
	uart_buffValid = 0;//������
	memset(uart_buff, 0, sizeof(uart_buff));//���buff
}


/// ����USART�����ж�
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
 * ��������GPRS_USART_Init
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
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

	/*	�����ж����ȼ� */
	GPRS_NVIC_Config();
	
	USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, DISABLE);//�����ж�
	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(GPRS_USARTx, USART_IT_RXNE, ENABLE);//�����ж�


	USART_Cmd(GPRS_USARTx, ENABLE);
	
	UartVarInit();
}


/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����GPRS_USARTx_printf()����
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
		if(Transmission_Sta)                                   //ָ��״̬
	{
		if(USART2->DR)                                     //����ֵ�ű���
		{
			SIM800C_RX_BUF[SIM800C_RX_STA]=USART2->DR;     //��ȡ����	
			SIM800C_RX_STA++;                              //SIM800C_RX_STA�������յ���������
		}
	}
	else                                            //����͸��״̬
	{	  
		if(SIM800C_RX_STA==0)		                    //�յ���һ������     
			TIM_Cmd(TIM3, ENABLE);                  //������ʱ��3
		SIM800C_RX_BUF[SIM800C_RX_STA]=USART2->DR;        //��ȡ����	
		TIM_SetCounter(TIM3, 0);                    //���㶨ʱ��3������			    		
		SIM800C_RX_STA++;                              //SIM800C_RX_STA�������յ��������� 
	}  
}

//��ȡ���յ������ݺͳ���
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
 * ��������GPRS_USARTx_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���2����GPRS_USARTx
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��GPRS_USARTx_printf( GPRS_USARTx, "\r\n this is a demo \r\n" );
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

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					USART_SendData(GPRS_USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(GPRS_USARTx,*s);
						while( USART_GetFlagStatus(GPRS_USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//ʮ����
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
