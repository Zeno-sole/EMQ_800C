#include "BSP_EMQ.h"


u8 Transmission_Sta;           //1：表示指令状态    0：表示透传状态
u8 serverLink_sta=0;           //0：未连接服务器    1：连接上服务器
u8 ping_flag=0;                //1：发送了ping报文  0：收到服务器回复
u8 EMQ_RES=0;                //1
u8 EMQ_success=0;                //1
u8 res;
u8 temp_buff[128];
u8 relay_flag = 3;
char *Cntr_CMD = "test";    //APP控制端的topic名称        订阅的主题          
char *Deal_CMD = "test1";    //开发板执行端的topic名称			发布的主题
char  Cntr_CMD_t[50] = "test";    //APP控制端的topic名称        订阅的主题  
char  Deal_CMD_t[50] = "test1";    //开发板执行端的topic名称			发布的主题
char *CMD1 = "y";          //   
char *CMD2 = "g";          //关机信号   
char *CMD3 = "k";          //开机信号
char *CMD4 = "AT+SWITCH1=1";     //第一路开关开：点亮D2
char *CMD5 = "AT+SWITCH1=0";     //第一路开关关：熄灭D2

uint32_t falsh_DEVICE_ID[2];	//write flash 缓存数组
uint32_t falsh_DEVICE_id[2];
uint32_t DEVICE_ID;
uint32_t DEVICE_IDc;
uint8_t  RxCmdBuff[128];   //配置命令接收缓存区
uint16_t RxCmdCount;      //接收数据计数器


void UartWriteBuffer_Config(const uint8_t *pBuffer,uint32_t NumByteToWrite){
	
	while(NumByteToWrite--)
	{
		USART_SendData(UART5, (uint8_t)*pBuffer++);
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
	}
}


void EMQ_Init(void){

	read_DEVICE_ID();
	sprintf(Cntr_CMD_t,"%d",DEVICE_ID);
	Cntr_CMD = Cntr_CMD_t;
	Uart5WriteBuffer_Config("\r\n", strlen("\r\n"));
	Uart5WriteBuffer_Config((u8 *)Cntr_CMD, strlen(Cntr_CMD));
	CLIENT_ID = Cntr_CMD;
	DEVICE_IDc = (DEVICE_ID * 10) + 1;
	sprintf(Deal_CMD_t,"%d",DEVICE_IDc);
	Deal_CMD = Deal_CMD_t;
	Uart5WriteBuffer_Config("\r\n", strlen("\r\n"));
	Uart5WriteBuffer_Config((u8 *)Deal_CMD, strlen(Deal_CMD));
	
	SIM800C_init();             //SIM800C串口初IO
 
	SIM800CUsart_init(115200);  //SIM800C串口初始化波特率115200


}
void EMQ_POLL(void){
//	OS_ERR err;
    char temp_sec[50];
/*--------------------------------------------------------------------------------------------*/
		/*                            连接上服务器后，开始处理MQTT的应用                              */
	    /*--------------------------------------------------------------------------------------------*/

		if(serverLink_sta==1)    
		{										  		
			/*--------------------------------------------------------------------------------------------*/
			/*                                      处理发送数据                                          */
			/*--------------------------------------------------------------------------------------------*/
			if(EMQTT_TxDataOutPtr != EMQTT_TxDataInPtr)        //if成立的话，说明发送缓冲区有数据了
			{
				printf("发送数据\r\n");
				EMQTT_TxData();                                //发送数据
				EMQTT_TxDataOutPtr+=EMQTT_UNIT;                //指针下移
				if(EMQTT_TxDataOutPtr==EMQTT_TxDataEndPtr)     //如果指针到缓冲区尾部了
					EMQTT_TxDataOutPtr = EMQTT_TxDataBuf[0];   //指针归位到缓冲区开头				
			}	
			/*--------------------------------------------------------------------------------------------*/
			/*                                      处理接收数据                                          */
			/*--------------------------------------------------------------------------------------------*/
			else if(EMQTT_RxDataOutPtr != EMQTT_RxDataInPtr)        //if成立的话，说明接收缓冲区有数据了
			{				
				
				/*-----------------------------------------------------*/
				/*                    处理CONNACK报文                  */
				/*-----------------------------------------------------*/
				if(EMQTT_RxDataOutPtr[1]==0x20)                //0x20开头报文是：确认连接请求报文
				{
				    if(EMQTT_CONNACK(EMQTT_RxDataOutPtr))      //处理该报文,返回值非0，表示错误
					{
						printf("CONNECT服务器失败，准备重启\r\n"); //串口显示信息
						NVIC_SystemReset();                        //重启
					}
					else
					{
						printf("CONNECT服务器成功\r\n");           //串口显示信息
						if(EMQTT_SUBSCRIBE(Cntr_CMD,0)!=0)
						{
							printf("SUBSCRIBE订阅topic报文错误\r\n");
						}
			
						ping_flag=0;                               //ping标志清零
						TIM4_Init();                               //30秒间隔ping
						TIM5_Init();                               //8秒ping超时检测			
						TIM2_Init();
						
					}					
				}
				
				/*-----------------------------------------------------*/
				/*            处理服务器发来的PUBLISH_Qs0报文          */
				/*-----------------------------------------------------*/
				else if(EMQTT_RxDataOutPtr[1]==0x30)                //0x30开头报文是：等级0的PUBLISH报文
				{
				    if(EMQTT_Deal_Qs0(EMQTT_RxDataOutPtr))          //处理该报文,返回0，表示正确
					{
						printf("非控制端报文，丢弃数据\r\n");
					}				
				}				
				
				/*-----------------------------------------------------*/
				/*                    处理PINGRESP报文                 */
				/*-----------------------------------------------------*/				
				else if(EMQTT_RxDataOutPtr[1]==0xD0)                //0xD0开头报文是：心跳响应报文
				{
					if(EMQTT_PINGRESP(EMQTT_RxDataOutPtr))          //处理该报文,返回值非0，表示错误
					{
						printf("ping响应错误，准备重启网络\r\n");   //串口显示信息
					}
					else
					{
						printf("收到ping响应\r\n");  
						ping_flag=0;                 //清楚标志
						TIM_Cmd(TIM5, DISABLE);      //关闭定时器5	
						TIM_SetCounter(TIM5, 0);     //清零定时器5计数器										
					}
				}
				
				/*-----------------------------------------------------*/
				/*                    处理SUBACK报文                   */
				/*-----------------------------------------------------*/
				else if(EMQTT_RxDataOutPtr[1]==0x90)           //0x90开头报文是：订阅回复报文
				{
				    res=EMQTT_SUBACK(EMQTT_RxDataOutPtr);      //处理该报文,返回值非0，表示错误
					
					switch(res)
					{
						case 0 : 
									EMQ_success = 1;
									TIM_Cmd(TIM2,ENABLE); 

									sprintf(temp_sec,"订阅topic成功\r\n");
									Uart5WriteBuffer_Config((u8 *)temp_sec, strlen(temp_sec));
									LINK_LED_PORT->ODR |= (LINK_LED_PIN);//亮灯
									memset(temp_sec, 0, sizeof(temp_sec));
				
								 break;
						case 1 : printf("订阅topic失败，准备重启\r\n");
								 delay_ms(500);
								 NVIC_SystemReset(); //重启
								 break;
						
						case 2 : printf("回复错误，准备重启\r\n");
								 delay_ms(500);
								 NVIC_SystemReset(); //重启
								 break;
						
						case 3 : printf("回复错误，准备重启\r\n");
								 delay_ms(500);
								 NVIC_SystemReset(); //重启
								 break;
					}					
				}
												
				/*-----------------------------------------------------*/
				/*                      处理未知报文                   */
				/*-----------------------------------------------------*/
				else
				{
					printf("未知报文\r\n");		
//					NVIC_SystemReset(); //重启					
				}								
				
				/*----------------处理完一个数据后，指针下移，一定注意不要搞乱顺序------------*/
				/*------------------------一定注意用if else if结构----------------------------*/
				EMQTT_RxDataOutPtr+=EMQTT_UNIT;                //指针下移
				if(EMQTT_RxDataOutPtr==EMQTT_RxDataEndPtr)     //如果指针到缓冲区尾部了
					EMQTT_RxDataOutPtr = EMQTT_RxDataBuf[0];   //指针归位到缓冲区开头
			}
			
			/*--------------------------------------------------------------------------------------------*/
			/*                                  处理控制的命令数据                                        */
			/*--------------------------------------------------------------------------------------------*/
			else if(EMQTT_CMDOutPtr != EMQTT_CMDInPtr)                 //if成立的话，说明命令缓冲区有数据了
			{
				printf("处理命令:%s\r\n",&EMQTT_CMDOutPtr[1]);

				if(!memcmp(&EMQTT_CMDOutPtr[1],CMD1,strlen(CMD1)))     //判断指令，如果是CMD1，就开始采集温湿度
				{   

					  printf("收到查询指令\r\n");
					  memset(temp_buff,0,128);
						sprintf((char *)temp_buff,"ok");          //构建上传数据			
						EMQTT_PUBLISH_Qs0(Deal_CMD,temp_buff);                            //发布数据 		
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD2,strlen(CMD2))) //判断指令，如果是CMD2，就开始采集光强度
				{   
					printf("收到关机指令\r\n");
					memset(temp_buff,0,128);
					sprintf((char *)temp_buff,"close");                  //构建上传数据
					EMQTT_PUBLISH_Qs0(Deal_CMD,temp_buff);                           //发布数据 
					relay_flag = 2;					
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD3,strlen(CMD3))) //判断指令，如果是CMD3，就开始采集大气压
				{   
					  printf("收到开机指令\r\n");
					  memset(temp_buff,0,128);
						sprintf((char *)temp_buff,"open");          //构建上传数据			
						EMQTT_PUBLISH_Qs0(Deal_CMD,temp_buff);                            //发布数据 
						relay_flag = 1;
						
					
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD4,strlen(CMD4))) //判断指令，如果是CMD4，点亮D2
				{                     
					printf("点亮D2\r\n");
//					LED0=0;
//					LED1 = 1;
					EMQTT_PUBLISH_Qs0(Deal_CMD,"+SW1:1");  //回复控制aPP，打开开关了
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD5,strlen(CMD5))) //判断指令，如果是CMD5，熄灭D2
				{                     
					printf("熄灭D2\r\n");
//					LED0=1;
//					LED1 = 0;
					EMQTT_PUBLISH_Qs0(Deal_CMD,"+SW1:0");  //回复控制aPP，打开开关了
				}
				
				else
				{
				 EMQTT_RxDataOutPtr = EMQTT_RxDataBuf[0];   //指针归位到缓冲区开头
					printf("未知命令准备重启\r\n");
					NVIC_SystemReset();                            //重启
				}
								
				EMQTT_CMDOutPtr+=EMQTT_UNIT;             //指针下移
				if(EMQTT_CMDOutPtr==EMQTT_CMDEndPtr)     //如果指针到缓冲区尾部了
					EMQTT_CMDOutPtr = EMQTT_CMDBuf[0];   //指针归位到缓冲区开头				
			}
				if(EMQ_success){
				
				 IWDG_Feed();
			}
								 
		}

}
void ParseCommand_Config(void){
//	OS_ERR  os_err;
	
	u8 i;
 char temp[50];
 char write_config_cmd[] = "rite:"; //写配置
 char read_config_cmd[] = "read";   //读配置
		  if(Find_usart5(write_config_cmd)){
//			OSTaskSuspend((OS_TCB*)&AppTaskCollectionTCB,&os_err);//挂起任务2
			i = StringFind((char *)RxCmdBuff,write_config_cmd)+4;
			printf("i=%d\r\n",i);
		 if((RxCmdBuff[i+1] != 0X00)&&(RxCmdBuff[i+2] != 0X00)&&(RxCmdBuff[i+3] != 0X00)&&(RxCmdBuff[i+4] != 0X00)&&(RxCmdBuff[i+5] != 0X00)&&(RxCmdBuff[i+6] != 0X00))
			{
				DEVICE_ID = ((RxCmdBuff[i+1]-0X30)*100000)+((RxCmdBuff[i+2]-0X30)*10000)+((RxCmdBuff[i+3]-0X30)*1000)+((RxCmdBuff[i+4]-0X30)*100)+((RxCmdBuff[i+5]-0X30)*10)+((RxCmdBuff[i+6]-0X30));
				
				sprintf(temp,"编号写入中请稍等...\r\n");		
			  Uart5WriteBuffer_Config((u8 *)temp, strlen(temp));
				memset(temp, 0, sizeof(temp));
				write_DEVICE_ID();
				sprintf(temp,"写入成功!编号为:%d\r\n",DEVICE_ID);
			  Uart5WriteBuffer_Config((u8 *)temp, strlen(temp));
				memset(temp, 0, sizeof(temp));
//				OSTaskResume((OS_TCB*)&AppTaskCollectionTCB,&os_err);	
			}
			else 
			{
				sprintf(temp,"获取错误请重新输入\r\n");	
				Uart5WriteBuffer_Config((u8 *)temp, strlen(temp));		
				memset(temp, 0, sizeof(temp));				
				
			}
				CLR_Buf5();
				
			
		}
	 else if(Find_usart5(read_config_cmd))
	 {
			CLR_Buf5();
		 printf("DEVICE_ID:%d\r\n",DEVICE_ID);
	 
	 }




}
void write_DEVICE_ID(void)
{
		falsh_DEVICE_ID[0] = 	DEVICE_ID;
		falsh_DEVICE_ID[1] =  10;
 		STMFLASH_Write(FLASH_DEVICE_ID_ADDR ,(u16*)falsh_DEVICE_ID,2);

}
void read_DEVICE_ID(void)
{
	  char temp_read[50];
		
		STMFLASH_Read(FLASH_DEVICE_ID_ADDR,(u16*)falsh_DEVICE_id,2);
		DEVICE_ID = 	falsh_DEVICE_id[0];
		if(falsh_DEVICE_id[0] > 0XFFFFF)   //Flash被清零之后，使用默认参数   
	{
		
	DEVICE_ID = 100170;									
		
	}
		sprintf(temp_read,"DEVICE_ID:%d\r\n",falsh_DEVICE_id[0]);
		Uart5WriteBuffer_Config((u8 *)temp_read, strlen(temp_read));	
	  memset(temp_read, 0, sizeof(temp_read));		

}
void RELAY_POLL(void){
		if(relay_flag == 1)
		{
			RELAY_PORT->ODR &= ~(RELAY_PIN);//Relay Off
			delay_s(1);
			RELAY_PORT->ODR |= RELAY_PIN;//Relay On
			relay_flag = 3;
		}
		else if(relay_flag == 2)
		{
			
			RELAY_PORT->ODR &= ~(RELAY_PIN);//Relay Off
			delay_s(8);
			RELAY_PORT->ODR |= RELAY_PIN;//Relay On
			relay_flag = 3;
		}

}
void delay_s(u8 num){
	
	delay_ms(num*1000);

}

/*
 * 设置 IWDG 的超时时间
 * Tout = prv/40 * rlv (s)
 *      prv可以是[4,8,16,32,64,128,256]
 * prv:预分频器值，取值如下：
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:预分频器值，取值范围为：0-0XFFF
 * 函数调用举例：
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s 超时溢出
 */

void IWDG_Config(uint8_t prv ,uint16_t rlv)
{	
	// 使能 预分频寄存器PR和重装载寄存器RLR可写
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// 设置预分频器值
	IWDG_SetPrescaler( prv );
	
	// 设置重装载寄存器值
	IWDG_SetReload( rlv );
	
	// 把重装载寄存器的值放到计数器中
	IWDG_ReloadCounter();
	
	// 使能 IWDG
	IWDG_Enable();	
}

// 喂狗
void IWDG_Feed(void)
{
	// 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
	// 当计数器的值减到0的时候会产生系统复位
	IWDG_ReloadCounter();
}
void EMQ_Connect(void){
	OS_ERR  os_err;
  u8 conts;
	IWDG_Config(IWDG_Prescaler_128 ,4095);    //13s
	printf("\r\n目前处于关机状态，准备开机\r\n");

	
/*-------------------------------------------------------------------------------------------------*/
	Transmission_Sta=1;                   //指令状态
	for(res=0;res<3;res++)                //检测是否处于AT指令状态
	{
		if(SIM800C_TestAT(1)==0)          //检测，超时时间500ms
		{
			printf("处于指令状态\r\n");   //串口提示数据
			break;                        //检测到处于AT指令状态，跳出for循环
		}	
	}
	if(res>=3)  printf("处于非指令状态\r\n");   
/*-------------------------------------------------------------------------------------------------*/
//	if(SIM800C_CPIN(20))                         //查询SIM卡状态，超时时间5s
//	{
//		while(1)
//		{
//			printf("SIM卡状态异常，请检查，卡是否插反\r\n");
//			delay_ms(100);
//			NVIC_SystemReset();  //重启	
//		}
//	}
//	printf("SIM卡状态正常\r\n");
/*-------------------------------------------------------------------------------------------------*/	
	conts = 10;
	while(SIM800C_CSQ(800)&&(conts>0)){
	
		conts--;
		delay_ms(500);
		printf("CSQ值过低\r\n");
	
	}

	delay_ms(100);
	IWDG_Feed();

	printf("CSQ正常\r\n");
///*-------------------------------------------------------------------------------------------------*/
//	if(SIM800C_SendCmd_OK("AT+CIPCLOSE=1",20))      //断开网络连接
//	{
//		printf("断开网络连接失败，准备重启\r\n");
//		delay_ms(500);
//		NVIC_SystemReset();  //重启	
//	}
//	  printf("断开网络连接成功\r\n");
///*-------------------------------------------------------------------------------------------------*/	
	if(SIM800C_CREG(800))                //等待有网络信号 50s
	{
		printf("查询网络信号，超时失败，准备重启\r\n");
		delay_ms(500);
		EMQ_Connect();  //重启		
	}
	printf("查询到网络信号\r\n");
/*-------------------------------------------------------------------------------------------------*/	
	if(SIM800C_SendCmd_OK("ATE0",50))               //关闭回显，超时时间5s
	{
		printf("关闭回显失败，准备重启\r\n");
		delay_ms(500);
		EMQ_Connect();  //重启			
	}
	printf("关闭回显成功\r\n");
/*-------------------------------------------------------------------------------------------------*/	
	if(SIM800C_SendCmd_OK("AT+GSMBUSY=1",50))      //设置GSM字符集，超时时间5s
	{
		printf("设置禁止电话呼入失败，准备重启\r\n");
		delay_ms(500);
		EMQ_Connect();  //重启	
	}
	printf("设置禁止电话呼入成功\r\n");

//	/*-------------------------------------------------------------------------------------------------*/
	if(SIM800C_SendCmd_OK("AT+CIPSHUT",50))      //关闭场景
	{
		printf("关闭场景失败，准备重启\r\n");
		delay_ms(500);
		EMQ_Connect();  //重启	
	}
	printf("关闭场景成功\r\n");
///*-------------------------------------------------------------------------------------------------*/
	if(SIM800C_SendCmd_OK("AT+CIPMODE=0",50))      //设置单链接模式，超时时间5s
	{
		printf("设置单链接模式失败，准备重启\r\n");
		delay_ms(500);
		EMQ_Connect();  //重启	
	}
	printf("设置单链接模式成功\r\n");
///*-------------------------------------------------------------------------------------------------*/		
	if(SIM800C_SendCmd_OK("AT+CIPMODE=1",50))      //设置透传模式，超时时间5s
	{
		printf("设置透传模式失败，准备重启\r\n");
		delay_ms(500);
		EMQ_Connect();  //重启	
	}
	printf("设置透传模式成功\r\n");
/*-------------------------------------------------------------------------------------------------*/
	
	delay_ms(200);
	res=SIM800C_GPRS(50);  //开始GPRS，连接服务器，超时30s
//	res = 0;
	switch(res)
	{
		case 0: printf("\r\n连接服务器成功\r\n");         //串口显示信息	

				TIM3_Init();		                      //定时器3初始化
				Transmission_Sta=0;                       //进入透传模式了
				SIM800C_RX_STA=0;                               //SIM800C串口状态标识清零                            
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);   //清空SIM800C串口接收缓冲区
				EMQTT_Init();                                   //EMQTT初始化
				serverLink_sta=1;                               //标志置位
//				LED1=0;		                                //点亮D1，表示连接上服务器
		    EMQTT_CONNECT();                                //发送连接报文		

		    OSTaskSuspend((OS_TCB*)&AppTaskGET1TCB,&os_err);//任务1执行5次后挂起任务2
				break;                                          //跳出
		  
		case 1: printf("查询附着GPRS超时，准备重启\r\n"); //串口显示信息
				delay_ms(500);
				EMQ_Connect();                       //重启
				break;                                    //跳出
		
		case 2: printf("开始任务超时，准备重启\r\n");     //串口显示信息
				delay_ms(500);
				EMQ_Connect();                       //重启
				break;                                    //跳出
		
		case 3: printf("建立GPRS连接超时，准备重启\r\n"); //串口显示信息
				delay_ms(500);
				EMQ_Connect();                       //重启
				break;                                    //跳出
		
		case 4: printf("获取IP地址超时，准备重启\r\n");   //串口显示信息
				delay_ms(500);
				EMQ_Connect();                       //重启
				break;                                    //跳出
				
		case 5: printf("连接服务器超时，准备重启\r\n");   //串口显示信息
        delay_ms(500);
				EMQ_Connect();                       //重启
				break;  							      //跳出
	}

}
void Tcp_Connect(void){
	OS_ERR  os_err;
	IWDG_Feed();
	if(SIM800C_SendCmd_OK("AT+CIPCLOSE=1",20))      //断开网络连接
	{
		printf("断开网络连接失败，准备重启\r\n");
		delay_ms(500);

	}
	else{
	  printf("断开网络连接成功\r\n");
	}
	if(SIM800C_SendCmd_OK("AT+CIPSHUT",50))      //关闭场景
	{
		printf("关闭场景失败，准备重启\r\n");
		delay_ms(500);
	
	}
	else{
	printf("关闭场景成功\r\n");
	}
		delay_ms(200);
	res=SIM800C_GPRS(50);  //开始GPRS，连接服务器，超时30s
//	res = 0;
	switch(res)
	{
		case 0: printf("\r\n连接服务器成功\r\n");         //串口显示信息	

				TIM3_Init();		                      //定时器3初始化
				Transmission_Sta=0;                       //进入透传模式了
				SIM800C_RX_STA=0;                               //SIM800C串口状态标识清零                            
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);   //清空SIM800C串口接收缓冲区
				EMQTT_Init();                                   //EMQTT初始化
				serverLink_sta=1;                               //标志置位
//				LED1=0;		                                //点亮D1，表示连接上服务器
		    EMQTT_CONNECT();                                //发送连接报文		
				TIM_Cmd(TIM4, ENABLE);   
		OSTaskSuspend((OS_TCB*)&AppTaskGET1TCB,&os_err);//任务1执行5次后挂起任务2
				break;                                          //跳出
		  
		case 1: printf("查询附着GPRS超时，准备重启\r\n"); //串口显示信息
				delay_ms(500);
//				EMQ_Connect();                       //重启
				break;                                    //跳出
		
		case 2: printf("开始任务超时，准备重启\r\n");     //串口显示信息
				delay_ms(500);
//				EMQ_Connect();                       //重启
				break;                                    //跳出
		
		case 3: printf("建立GPRS连接超时，准备重启\r\n"); //串口显示信息
				delay_ms(500);
//				EMQ_Connect();                       //重启
				break;                                    //跳出
		
		case 4: printf("获取IP地址超时，准备重启\r\n");   //串口显示信息
				delay_ms(500);
//				EMQ_Connect();                       //重启
				break;                                    //跳出
				
		case 5: printf("连接服务器超时，准备重启\r\n");   //串口显示信息
        delay_ms(500);
//				EMQ_Connect();                       //重启
				break;  							      //跳出
	}
	

}
void ResConnect(void){
	OS_ERR  os_err;
	OSTaskResume((OS_TCB*)&AppTaskGET1TCB,&os_err);	

}
