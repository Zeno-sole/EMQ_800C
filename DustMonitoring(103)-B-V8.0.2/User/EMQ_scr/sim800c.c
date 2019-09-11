/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "BSP_EMQ.h"
#include "sim800c.h"
#include "sim800c_usart.h"	
//#include "delay.h"
//#include "usart1.h"
//#include "led.h"
#include <string.h>
/*-------------------------------------------------*/
/*函数名：初始化SIM800C                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SIM800C_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                                   //定义一个IO端口参数结构体
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);  			   //使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                              //初始化PC0端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   					   //推免输出方式
	GPIO_Init(GPIOD, &GPIO_InitStructure);            					   //初始化PC0.
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;                             //初始化PC1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   					   //下拉输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);            					   //初始化PC1
}

/*-------------------------------------------------*/
/*函数名：SIM800C开机                              */
/*参  数：无                                       */
/*返回值：0:开机成功  1:开机失败                   */
/*-------------------------------------------------*/
/*
u8 SIM800C_Power(void)
{
	u8 res;
//	
//	if(POWER_STA==0)   //目前处于关机状态
//	{		
//		printf("\r\n目前处于关机状态，准备开机\r\n");
//		POWER_KEY=0;
//		delay_ms(1500);
//		POWER_KEY=1;
//	}
//	else                  //目前处于开机状态
//	{
//		printf("\r\n目前处于开机状态，准备重启\r\n");
//		
//		POWER_KEY=0;      //关机
//		delay_ms(1800);
//		POWER_KEY=1;

////		delay_ms(1500);   //间隔

////		POWER_KEY=1;      //开机
////		delay_ms(1500);
////		POWER_KEY=0;		
////		delay_ms(1500);
//	}
//	
//	delay_ms(1500);	
//	if(POWER_STA)
//	{
//		printf("开机成功\r\n");
//		res=0;
//	}
//	else
//	{
//		printf("开机失败\r\n");
//		res=1;
//	}


}
*/
/*-------------------------------------------------*/
/*函数名：检测AT指令状态                           */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0:AT指令态  1：非指令态                  */
/*-------------------------------------------------*/
u8 SIM800C_TestAT(u16 timeout)
{
	u16 time=0;                                       //超时计数器
	
	SIM800C_RX_STA=0;                                    //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);        //清空SIM800C串口接收缓冲区 
	SIM800C_printf("AT\r\n");                            //发送AT测试指令
	
	time=0;                                              //超时计数器清零	
    while(time<timeout)                                  //等待超时与否
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//如果收到OK，表示处于AT模式
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
			break;       //跳出while循环
		}
		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1   
	}
	if(time>=timeout)    //超时，表示处于透传状态
		return 1;
	
	return 0;
}

/*-------------------------------------------------*/
/*函数名：发送指令，期待的回复是OK                 */
/*参  数：cmd：指令                                */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_SendCmd_OK(u8 *cmd, u16 timeout)
{
    u16 time;                                   //超时计数器
	
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("%s\r\n",cmd);                  //发送指令
	time=0;                                        //超时计数器清零
	
	while(time<timeout)                         //等待超时与否
	{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//如果接收到OK表示指令成功
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
			break;       //跳出while循环
		}
		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 1;   //超时错误，返回1
	else return 0;		         //无错，返回0
}

/*-------------------------------------------------*/
/*函数名：CPIN指令                                 */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_CPIN(u16 timeout)
{
    u16 time;                                      //超时计数器
	
//	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
//	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
//	SIM800C_printf("AT+CPIN?\r\n");                //发送指令
	time=0;                                        //超时计数器清零
	
	while(time<timeout)                         //等待超时与否
	{
		
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零 	
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("AT+CPIN?\r\n");                //发送指令
	delay_ms(50);   //延时100ms
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"READY"))//如果接收到READY表示SIM卡状态OK
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
			break;       //跳出while循环
		}
		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 1;   //超时错误，返回1
	else return 0;		         //无错，返回0	
}
/*-------------------------------------------------*/
/*函数名：CSQ指令                                 */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_CSQ(u16 timeout)
{
    u16 time;	//超时计数器
//		u8 i=0,j=0;
//	u8 CSQ = 0;
//	char temp[50]={0};
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("AT+CSQ\r\n");                //发送指令
	time=0;                                        //超时计数器清零
	
	while(time<timeout)                         //等待超时与否
//	while(1)
	{	

	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("AT+CSQ\r\n");                //发送指令
	delay_ms(50);   //延时100ms
	UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
	if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"+CSQ: "))//如果接收到READY表示SIM卡状态OK
	{
		
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));


				
							if((StringFind((char *)SIM800C_RX_BUF,"+CSQ: 0,0")) < 0)
			{
//				if((StringFind((char *)SIM800C_RX_BUF,",5")) > 0)
							break;     //跳出while循环
			}
		}




		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 1;   //超时错误，返回1
	else return 0;		         //无错，返回0	
}
/*-------------------------------------------------*/
/*函数名：等待查询注册上SIM卡                      */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_QueryStatus(u16 timeout)
{
    u16 time;                                      //超时计数器
	
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	time=0;                                        //超时计数器清零
	
	while(time<timeout)                            //等待超时与否
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"Call Ready\r\n\r\nSMS Ready"))//如果接收到Call Ready\r\n\r\nSMS Ready表示注册上SIM卡
		{
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
		break;       //跳出while循环
		}
		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 1;   //超时错误，返回1
	else return 0;		         //无错，返回0	
}

/*-------------------------------------------------*/
/*函数名：ATD指令，拨打电话                        */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_ATD(char *num, u16 timeout)
{
    u16 time;                                      //超时计数器
	
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("ATD%s;\r\n",num);              //发送指令
	time=0;                                        //超时计数器清零
	
	while(time<timeout)                            //等待超时与否
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO DIALTONE"))//如果接收到NO DIALTONE,表示拨号失败
			return 1;    //返回1
		else if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO CARRIER"))//如果接收到NO CARRIER,表示连接失败
			return 2;    //返回2
		else if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO ANSWER"))//如果接收到NO ANSWER,表示无人应答 
			return 3;    //返回3
		else if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//如果接收到OK,表示应答接听了 
			break;       //跳出while循环		
		
//		LED1=!LED1;
		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 4;   //超时错误，返回4
	else
	{
		SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
		time=0;                                        //超时计数器清零
			
		while(time<timeout)                            //等待超时与否
		{
			if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO CARRIER"))//如果接收到NO CARRIER，表示应答方挂断
				break;       //跳出while循环
			delay_ms(100);   //延时100ms
			time++;          //超时计数器+1
		}
	    if(time>=timeout)return 5;   //超时错误，返回1
	}
	
	return 0;		                //无错，返回0	
}

/*-------------------------------------------------*/
/*函数名：CMGS指令，发短信                         */
/*参  数：num：电话                                */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_CMGS(char *num, u16 timeout)
{
    u16 time;                                      //超时计数器
	
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("AT+CMGS=\"%s\"\r\n",num);      //发送指令
	time=0;                                        //超时计数器清零
	
	while(time<timeout)                            //等待超时与否
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)">"))//如果接收到>,表示可以发送短信内容
		{
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));			
			break;       //跳出while循环
		}
//		LED0=!LED0;		
		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 1;   //超时错误，返回1
	else
	{
		SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲
		SIM800C_printf("Warning!!!",num);              //发送短信数据
		SIM800C_TxChar(0x1A);                          //发结束符
		time=0;                                        //超时计数器清零
			
		while(time<timeout)                            //等待超时与否
		{
			if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//如果接收到OK，表示发送短信成功
				break;       //跳出while循环
			delay_ms(100);   //延时100ms
			time++;          //超时计数器+1
		}
	    if(time>=timeout)return 2;   //超时错误，返回2
	}
	
	return 0;		                //无错，返回0	
}

/*-------------------------------------------------*/
/*函数名：GPRS联网服务器                           */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_GPRS(u16 timeout)
{
    u16 time;                                      //超时计数器
	
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("AT+CGATT?\r\n");               //发送指令，查看GPRS附着情况
	printf("AT+CGATT?\r\n");
	time=0;                                        //超时计数器清零
	UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
	while(time<timeout)                            //等待超时与否
	{
		
	SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	SIM800C_printf("AT+CGATT?\r\n");               //发送指令，查看GPRS附着情况
	delay_ms(50);   //延时100ms

		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"+CGATT: 1"))//如果接收到+CGATT:1\r\n\r\nOK,表示可以GPRS通信了 
		{
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
		break;       //跳出while循环	
		}
		
		delay_ms(100);   //延时100ms
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 1;   //超时错误，返回1
	else
	{
		SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零 
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));		
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲
		SIM800C_printf("AT+CSTT\r\n");                 //发送指令，开始任务
		printf("AT+CSTT\r\n");
		time=0;                                        //超时计数器清零
			
		while(time<timeout)                            //等待超时与否
		{
			if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//如果接收到OK，表示成功
			{
				UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
				break;       //跳出while循环
			}
			delay_ms(100);   //延时100ms
			time++;          //超时计数器+1
		}
	    if(time>=timeout)return 2;   //超时错误，返回2
		else
		{
			SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零   
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));			
			memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲
			SIM800C_printf("AT+CIICR\r\n");                //发送指令，建立GPRS连接
			printf("AT+CIICR\r\n");
			time=0;                                        //超时计数器清零
				
			while(time<timeout)                            //等待超时与否
			{
				
				
				if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//如果接收到OK，表示成功
				{
					UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
					break;       //跳出while循环
				}
				delay_ms(100);   //延时100ms
				time++;          //超时计数器+1
			}
			if(time>=timeout)return 3;   //超时错误，返回3
			else
			{
				SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲
				SIM800C_printf("AT+CIFSR\r\n");                //发送指令，获取IP地址
				time=0;                                        //超时计数器清零
					
				while(time<timeout)                            //等待超时与否
				{
					
					if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"."))//如果接收到.，表示成功
					{
						UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
						break;       //跳出while循环
					}
					delay_ms(100);   //延时100ms
					time++;          //超时计数器+1
				}
				if(time>=timeout)return 4;   //超时错误，返回4
				else
				{
					printf("获取的IP地址：%s\r\n",SIM800C_RX_BUF);
					SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
					memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲
					SIM800C_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",SERVER_IP,SERVER_PORT); //发送指令，连接服务器
					time=0;                                        //超时计数器清零
						
					while(time<timeout)                            //等待超时与否
					{
			
						if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"CONNECT"))//如果接收到CONNECT，表示连接服务器成功
						{
							UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
							break;       //跳出while循环
						}
						delay_ms(100);   //延时100ms
						time++;          //超时计数器+1
					}
					if(time>=timeout)return 5;   //超时错误，返回5
				}
			}
		}
	}
	
	return 0;		                //无错，返回0	
}

/*-------------------------------------------------*/
/*函数名：查询信号                                 */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
u8 SIM800C_CREG(u16 timeout)
{
    u16 time;                                   //超时计数器
	
	time=0;                                        //超时计数器清零	
	while(time<timeout)                         //等待超时与否
	{
			SIM800C_RX_STA=0;                              //SIM800C串口状态标识清零                            
	    memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //清空SIM800C串口接收缓冲区 
	    SIM800C_printf("AT+CREG?\r\n");                //发送指令
			delay_ms(100);                                //延时1s
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));	
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"+CREG: 0,1"))//如果接收到+CREG:0,1注册网络成功
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));		
			break;       //跳出while循环
		}
		time++;          //超时计数器+1
	}
	if(time>=timeout)return 1;   //超时错误，返回1
	else return 0;		         //无错，返回0
}


//int32_t StringFind(const char *pSrc, const char *pDst)  
//{
//    int i, j;  
//    for (i=0; pSrc[i]!='\0'; i++)  
//    {  
//        if(pSrc[i]!=pDst[0])  
//            continue;         
//        j = 0;  
//        while(pDst[j]!='\0' && pSrc[i+j]!='\0')  
//        {  
//            j++;  
//            if(pDst[j]!=pSrc[i+j])
//            break;
//        }
//        if(pDst[j]=='\0')
//            return i;
//    }
//    return -1;  
//}
