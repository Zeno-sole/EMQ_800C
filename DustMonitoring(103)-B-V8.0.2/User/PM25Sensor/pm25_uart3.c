#include "bsp.h"

//PM2.5/PM10/温湿度/噪声
uint8_t PM25_RxBuff[256];
uint16_t PM25_RxBuffCount;

static void UartVarInit(void);
static void USART_Configuration(void);
static void ConfigUartNVIC(void);

void PM25_UART3_Init(void){
	UartVarInit();
	USART_Configuration();
	ConfigUartNVIC();
}

static void UartVarInit(void){
	PM25_RxBuffCount=0;
	memset(PM25_RxBuff, 0, sizeof(PM25_RxBuff));
}

static void USART_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);/* Enable GPIO clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);/*Enable USART clock */
	
	/* Configure USART3 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* USART3 configured as follow:
        - BaudRate = 9600 baud  
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
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //收发模式
	USART_Init(USART3, &USART_InitStructure);/* Configure USART3 */
	
	/* Enable USART3 Receive and Transmit interrupts */
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//接收中断
	
	/* Enable the USART3*/
	USART_Cmd(USART3, ENABLE);
	
	USART_ClearFlag(USART3,USART_FLAG_TC);
	

}

static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART3_NVIC_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void Open_Measurement(void){
	//16 02 0C 02 DA 

	
	u8 open_[]  ={0x11,0x03,0x0c,0x02,0x1e,0xc0};
	u8 open_1[] ={0x16,0x02,0x0c,0x02,0xda};		
	Uart3WriteBuffer_Config(open_,strlen((char *)open_));
	delay_ms(300);
	

		if(Find_usart3((char *)open_1)){
			
			CLR_Buf3();
			printf("开启测量成功！\r\n");
		
		}
	
	

}
void Close_Measurement(void){
	//16 02 0C 01 DB
	u8 open_cont = 5;
	
	u8 open_[6]  ={0x11,0x03,0x0c,0x01,0x1e,0xc1};
	u8 open_1[] ={0x16,0x02,0x0c,0x01,0xdb};		
	Uart3WriteBuffer_Config(open_,6);
	delay_ms(300);
	

		if(Find_usart3((char *)open_1)){
			
			CLR_Buf3();
			printf("关闭测量成功！\r\n");

		}

	
	

}

//void ParsePM25Command(void){
//		//11 02 0B 07 DB  读取测量结果


//	u8 get_data[]={0x11,0x02,0x0b,0x07,0x0db};
//	u8 get_start[] ={0x16,0x35,0x0b};
//	CLR_Buf3();
//	Uart3WriteBuffer_Config(get_data,strlen((char *)get_data));
//	delay_ms(500);	
//	if(Find_usart3((char *)get_start)){
//			
//			mySensorDatas.PM2_5 = (PM25_RxBuff[9] << 8) | PM25_RxBuff[10];//PM2.5
//			mySensorDatas.PM10  = (PM25_RxBuff[13] << 8) | PM25_RxBuff[14];//PM10		
//			mySensorDatas.TSP   = ((float)(mySensorDatas.PM10) / 0.7) + 0.5;//TSP
////			printf("PM2_5 = %d\r\n",mySensorDatas.PM2_5);
////			printf("PM10 = %d\r\n",mySensorDatas.PM10);				
//			CLR_Buf3();


//		}


//}



void ParsePM25Command(void){
	uint16_t i;
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//禁止接收中断
	for(i = 0; i < (sizeof(PM25_RxBuff) - 32); i++){
		if(PM25_RxBuff[i] == 0x42 && PM25_RxBuff[i+1] == 0x4D){
//			mySensorDatas.TEMP  = (PM25_RxBuff[i+24] << 8) | PM25_RxBuff[i+25];//温度
//			mySensorDatas.HUMI  = (PM25_RxBuff[i+26] << 8) | PM25_RxBuff[i+27];//湿度
			mySensorDatas.PM2_5 = (PM25_RxBuff[i+6] << 8) | PM25_RxBuff[i+7];//PM2.5
			mySensorDatas.PM10  = (PM25_RxBuff[i+8] << 8) | PM25_RxBuff[i+9];//PM10
			mySensorDatas.TSP   = ((float)(mySensorDatas.PM10) / 0.7) + 0.5;//TSP
			PM25_RxBuffCount = 0;
			memset(PM25_RxBuff, 0, sizeof(PM25_RxBuff));
			break;
		}
	}
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//接收中断
}
