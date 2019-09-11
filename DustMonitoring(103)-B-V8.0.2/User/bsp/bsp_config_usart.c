#include "bsp.h"

static void UartVarInit(void);
static void USART_Configuration(void);
static void ConfigUartNVIC(void);

extern OS_TCB				   AppTaskStartTCB;
extern OS_TCB          AppTaskCollectionTCB;

uint8_t  RxDataBuff[256];  //配置数据接收缓存区
//uint8_t  RxCmdBuff[128];   //配置命令接收缓存区
uint16_t RxDataCount;      //接收数据计数器
//uint16_t RxCmdCount;       //接收命令计数器
uint8_t  RxStateConfig;    //接收完一帧数据
uint8_t  CmdFlag;          //0表示命令,1表示数据

const char write_config_cmd[] = "write\r\n"; //写配置
const char read_config_cmd[] = "read\r\n";   //读配置
const char read_sysinfo_cmd[] = "sysinfo\r\n"; //查看CPU使用率和任务内存使用率

void BSP_InitUart_Config(void){
	UartVarInit();
	USART_Configuration();
	ConfigUartNVIC();
}
/*
void ParseCommand_Config(void){
	
	if(RxStateConfig == 1){
		RxStateConfig = 0;
		RxCmdCount = 0;//接收命令计数器清零

		if(memcmp((char*)RxCmdBuff, write_config_cmd, sizeof(write_config_cmd)) == 0){
			
			memset(RxDataBuff, 0 ,sizeof(RxDataBuff));
			RxDataCount = 0;//接收数据计数器清零
			CmdFlag = 1;//开始接收数据
			UartWriteBuffer_Config("请输入设备编号：\r\n", strlen("请输入设备编号：\r\n"));
			
		}else if(memcmp((char*)RxCmdBuff, read_config_cmd, sizeof(read_config_cmd)) == 0){
			
			memset(RxDataBuff, 0 ,sizeof(RxDataBuff));
			
			FLASH_ReadHalfWord(RxDataBuff, sizeof(RxDataBuff));//读出数据
			UartWriteBuffer_Config(RxDataBuff, strlen((char *)RxDataBuff));//发送数据
			
		}else if(memcmp((char*)RxCmdBuff, read_sysinfo_cmd, sizeof(read_sysinfo_cmd)) == 0){
			
			#if(OS_CFG_STAT_TASK_EN > 0u)
				OS_ERR	err;
				CPU_STK_SIZE free,used;
				printf("CPU Usage: %.3f%%\r\n",(1-((float)OSStatTaskCtr/(float)OSStatTaskCtrMax))*100);
			
				OSTaskStkChk(&AppTaskStartTCB,&free,&used,&err);
				printf("Start---------RAM %-5dB/%-5dB Usage: %-3d%%\r\n",free,used,(used*100)/(free+used));
				OSTaskStkChk(&AppTaskCollectionTCB,&free,&used,&err);
				printf("Collection----RAM %-5dB/%-5dB Usage: %-3d%%\r\n",free,used,(used*100)/(free+used));	
			#endif
			
		}
		
		memset(RxCmdBuff, 0, sizeof(RxCmdBuff));
	}
}
*/
static void UartVarInit(void){
	
	RxDataCount = 0;
	RxCmdCount = 0;
	RxStateConfig = 0;
	CmdFlag = 0;
	memset(RxCmdBuff, 0, sizeof(RxCmdBuff));
	memset(RxDataBuff, 0, sizeof(RxDataBuff));
}

static void USART_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);/* Enable GPIO clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);/*Enable UART5 clock */

	/* Configure UART5 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure UART5 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  /* UART5 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	/* Configure UART5 */
  USART_Init(UART5, &USART_InitStructure);
	
  /* Enable UART5 Receive and Transmit interrupts */
//	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);//空闲中断
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//接收中断
	
  /* Enable the UART5*/
  USART_Cmd(UART5, ENABLE);
	
	USART_ClearFlag(UART5, USART_FLAG_TC);
}

static void ConfigUartNVIC(void){
	
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART5_NVIC_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/*
void UartWriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite){
	
	while(NumByteToWrite--)
	{
		USART_SendData(UART5, (uint8_t)*pBuffer++);
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
	}
}
*/
void Uart5WriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite){
	
	while(NumByteToWrite--)
	{
		USART_SendData(UART5, (uint8_t)*pBuffer++);
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
	}
}
void Uart3WriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite){
	
	while(NumByteToWrite--)
	{
		USART_SendData(USART3, (uint8_t)*pBuffer++);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	}
}
void CLR_Buf5(void)
{

		memset(RxCmdBuff, 0, sizeof(RxCmdBuff));
    RxCmdCount = 0;              //接收字符串的起始存储位置
}
u8 Find_usart5(char *a)    //查询接收缓存数组内容 
{ 
  if(strstr((char *)RxCmdBuff,a)!=NULL)
	    return 1;
	else
			return 0;
}
u8 Find_usart3(char *a)    //查询接收缓存数组内容 
{ 
  if(strstr((char *)PM25_RxBuff,a)!=NULL)
	    return 1;
	else
			return 0;
}
void CLR_Buf3(void)
{

		memset(PM25_RxBuff, 0, sizeof(PM25_RxBuff));
    PM25_RxBuffCount = 0;              //接收字符串的起始存储位置
}
/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
	/* 写一个字节到UART5 */
	USART_SendData(UART5, (uint8_t) ch);

	/* 等待发送结束 */
	while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);		

	return ch;
}
