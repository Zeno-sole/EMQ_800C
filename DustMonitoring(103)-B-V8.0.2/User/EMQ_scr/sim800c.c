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
/*����������ʼ��SIM800C                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SIM800C_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                                   //����һ��IO�˿ڲ����ṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);  			   //ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                              //��ʼ��PC0�˿�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   					   //���������ʽ
	GPIO_Init(GPIOD, &GPIO_InitStructure);            					   //��ʼ��PC0.
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;                             //��ʼ��PC1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   					   //��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);            					   //��ʼ��PC1
}

/*-------------------------------------------------*/
/*��������SIM800C����                              */
/*��  ������                                       */
/*����ֵ��0:�����ɹ�  1:����ʧ��                   */
/*-------------------------------------------------*/
/*
u8 SIM800C_Power(void)
{
	u8 res;
//	
//	if(POWER_STA==0)   //Ŀǰ���ڹػ�״̬
//	{		
//		printf("\r\nĿǰ���ڹػ�״̬��׼������\r\n");
//		POWER_KEY=0;
//		delay_ms(1500);
//		POWER_KEY=1;
//	}
//	else                  //Ŀǰ���ڿ���״̬
//	{
//		printf("\r\nĿǰ���ڿ���״̬��׼������\r\n");
//		
//		POWER_KEY=0;      //�ػ�
//		delay_ms(1800);
//		POWER_KEY=1;

////		delay_ms(1500);   //���

////		POWER_KEY=1;      //����
////		delay_ms(1500);
////		POWER_KEY=0;		
////		delay_ms(1500);
//	}
//	
//	delay_ms(1500);	
//	if(POWER_STA)
//	{
//		printf("�����ɹ�\r\n");
//		res=0;
//	}
//	else
//	{
//		printf("����ʧ��\r\n");
//		res=1;
//	}


}
*/
/*-------------------------------------------------*/
/*�����������ATָ��״̬                           */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0:ATָ��̬  1����ָ��̬                  */
/*-------------------------------------------------*/
u8 SIM800C_TestAT(u16 timeout)
{
	u16 time=0;                                       //��ʱ������
	
	SIM800C_RX_STA=0;                                    //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);        //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("AT\r\n");                            //����AT����ָ��
	
	time=0;                                              //��ʱ����������	
    while(time<timeout)                                  //�ȴ���ʱ���
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//����յ�OK����ʾ����ATģʽ
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
			break;       //����whileѭ��
		}
		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1   
	}
	if(time>=timeout)    //��ʱ����ʾ����͸��״̬
		return 1;
	
	return 0;
}

/*-------------------------------------------------*/
/*������������ָ��ڴ��Ļظ���OK                 */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_SendCmd_OK(u8 *cmd, u16 timeout)
{
    u16 time;                                   //��ʱ������
	
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("%s\r\n",cmd);                  //����ָ��
	time=0;                                        //��ʱ����������
	
	while(time<timeout)                         //�ȴ���ʱ���
	{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//������յ�OK��ʾָ��ɹ�
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
			break;       //����whileѭ��
		}
		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 1;   //��ʱ���󣬷���1
	else return 0;		         //�޴�����0
}

/*-------------------------------------------------*/
/*��������CPINָ��                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_CPIN(u16 timeout)
{
    u16 time;                                      //��ʱ������
	
//	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
//	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
//	SIM800C_printf("AT+CPIN?\r\n");                //����ָ��
	time=0;                                        //��ʱ����������
	
	while(time<timeout)                         //�ȴ���ʱ���
	{
		
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ���� 	
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("AT+CPIN?\r\n");                //����ָ��
	delay_ms(50);   //��ʱ100ms
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"READY"))//������յ�READY��ʾSIM��״̬OK
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
			break;       //����whileѭ��
		}
		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 1;   //��ʱ���󣬷���1
	else return 0;		         //�޴�����0	
}
/*-------------------------------------------------*/
/*��������CSQָ��                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_CSQ(u16 timeout)
{
    u16 time;	//��ʱ������
//		u8 i=0,j=0;
//	u8 CSQ = 0;
//	char temp[50]={0};
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("AT+CSQ\r\n");                //����ָ��
	time=0;                                        //��ʱ����������
	
	while(time<timeout)                         //�ȴ���ʱ���
//	while(1)
	{	

	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("AT+CSQ\r\n");                //����ָ��
	delay_ms(50);   //��ʱ100ms
	UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
	if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"+CSQ: "))//������յ�READY��ʾSIM��״̬OK
	{
		
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));


				
							if((StringFind((char *)SIM800C_RX_BUF,"+CSQ: 0,0")) < 0)
			{
//				if((StringFind((char *)SIM800C_RX_BUF,",5")) > 0)
							break;     //����whileѭ��
			}
		}




		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 1;   //��ʱ���󣬷���1
	else return 0;		         //�޴�����0	
}
/*-------------------------------------------------*/
/*���������ȴ���ѯע����SIM��                      */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_QueryStatus(u16 timeout)
{
    u16 time;                                      //��ʱ������
	
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	time=0;                                        //��ʱ����������
	
	while(time<timeout)                            //�ȴ���ʱ���
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"Call Ready\r\n\r\nSMS Ready"))//������յ�Call Ready\r\n\r\nSMS Ready��ʾע����SIM��
		{
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
		break;       //����whileѭ��
		}
		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 1;   //��ʱ���󣬷���1
	else return 0;		         //�޴�����0	
}

/*-------------------------------------------------*/
/*��������ATDָ�����绰                        */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_ATD(char *num, u16 timeout)
{
    u16 time;                                      //��ʱ������
	
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("ATD%s;\r\n",num);              //����ָ��
	time=0;                                        //��ʱ����������
	
	while(time<timeout)                            //�ȴ���ʱ���
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO DIALTONE"))//������յ�NO DIALTONE,��ʾ����ʧ��
			return 1;    //����1
		else if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO CARRIER"))//������յ�NO CARRIER,��ʾ����ʧ��
			return 2;    //����2
		else if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO ANSWER"))//������յ�NO ANSWER,��ʾ����Ӧ�� 
			return 3;    //����3
		else if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//������յ�OK,��ʾӦ������� 
			break;       //����whileѭ��		
		
//		LED1=!LED1;
		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 4;   //��ʱ���󣬷���4
	else
	{
		SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
		time=0;                                        //��ʱ����������
			
		while(time<timeout)                            //�ȴ���ʱ���
		{
			if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"NO CARRIER"))//������յ�NO CARRIER����ʾӦ�𷽹Ҷ�
				break;       //����whileѭ��
			delay_ms(100);   //��ʱ100ms
			time++;          //��ʱ������+1
		}
	    if(time>=timeout)return 5;   //��ʱ���󣬷���1
	}
	
	return 0;		                //�޴�����0	
}

/*-------------------------------------------------*/
/*��������CMGSָ�������                         */
/*��  ����num���绰                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_CMGS(char *num, u16 timeout)
{
    u16 time;                                      //��ʱ������
	
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("AT+CMGS=\"%s\"\r\n",num);      //����ָ��
	time=0;                                        //��ʱ����������
	
	while(time<timeout)                            //�ȴ���ʱ���
	{
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)">"))//������յ�>,��ʾ���Է��Ͷ�������
		{
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));			
			break;       //����whileѭ��
		}
//		LED0=!LED0;		
		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 1;   //��ʱ���󣬷���1
	else
	{
		SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ���
		SIM800C_printf("Warning!!!",num);              //���Ͷ�������
		SIM800C_TxChar(0x1A);                          //��������
		time=0;                                        //��ʱ����������
			
		while(time<timeout)                            //�ȴ���ʱ���
		{
			if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//������յ�OK����ʾ���Ͷ��ųɹ�
				break;       //����whileѭ��
			delay_ms(100);   //��ʱ100ms
			time++;          //��ʱ������+1
		}
	    if(time>=timeout)return 2;   //��ʱ���󣬷���2
	}
	
	return 0;		                //�޴�����0	
}

/*-------------------------------------------------*/
/*��������GPRS����������                           */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_GPRS(u16 timeout)
{
    u16 time;                                      //��ʱ������
	
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("AT+CGATT?\r\n");               //����ָ��鿴GPRS�������
	printf("AT+CGATT?\r\n");
	time=0;                                        //��ʱ����������
	UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
	while(time<timeout)                            //�ȴ���ʱ���
	{
		
	SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	SIM800C_printf("AT+CGATT?\r\n");               //����ָ��鿴GPRS�������
	delay_ms(50);   //��ʱ100ms

		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"+CGATT: 1"))//������յ�+CGATT:1\r\n\r\nOK,��ʾ����GPRSͨ���� 
		{
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
		break;       //����whileѭ��	
		}
		
		delay_ms(100);   //��ʱ100ms
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 1;   //��ʱ���󣬷���1
	else
	{
		SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ���� 
		UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));		
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ���
		SIM800C_printf("AT+CSTT\r\n");                 //����ָ���ʼ����
		printf("AT+CSTT\r\n");
		time=0;                                        //��ʱ����������
			
		while(time<timeout)                            //�ȴ���ʱ���
		{
			if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//������յ�OK����ʾ�ɹ�
			{
				UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
				break;       //����whileѭ��
			}
			delay_ms(100);   //��ʱ100ms
			time++;          //��ʱ������+1
		}
	    if(time>=timeout)return 2;   //��ʱ���󣬷���2
		else
		{
			SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����   
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));			
			memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ���
			SIM800C_printf("AT+CIICR\r\n");                //����ָ�����GPRS����
			printf("AT+CIICR\r\n");
			time=0;                                        //��ʱ����������
				
			while(time<timeout)                            //�ȴ���ʱ���
			{
				
				
				if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"OK"))//������յ�OK����ʾ�ɹ�
				{
					UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
					break;       //����whileѭ��
				}
				delay_ms(100);   //��ʱ100ms
				time++;          //��ʱ������+1
			}
			if(time>=timeout)return 3;   //��ʱ���󣬷���3
			else
			{
				SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ���
				SIM800C_printf("AT+CIFSR\r\n");                //����ָ���ȡIP��ַ
				time=0;                                        //��ʱ����������
					
				while(time<timeout)                            //�ȴ���ʱ���
				{
					
					if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"."))//������յ�.����ʾ�ɹ�
					{
						UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
						break;       //����whileѭ��
					}
					delay_ms(100);   //��ʱ100ms
					time++;          //��ʱ������+1
				}
				if(time>=timeout)return 4;   //��ʱ���󣬷���4
				else
				{
					printf("��ȡ��IP��ַ��%s\r\n",SIM800C_RX_BUF);
					SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
					memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ���
					SIM800C_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",SERVER_IP,SERVER_PORT); //����ָ����ӷ�����
					time=0;                                        //��ʱ����������
						
					while(time<timeout)                            //�ȴ���ʱ���
					{
			
						if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"CONNECT"))//������յ�CONNECT����ʾ���ӷ������ɹ�
						{
							UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));
							break;       //����whileѭ��
						}
						delay_ms(100);   //��ʱ100ms
						time++;          //��ʱ������+1
					}
					if(time>=timeout)return 5;   //��ʱ���󣬷���5
				}
			}
		}
	}
	
	return 0;		                //�޴�����0	
}

/*-------------------------------------------------*/
/*����������ѯ�ź�                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
u8 SIM800C_CREG(u16 timeout)
{
    u16 time;                                   //��ʱ������
	
	time=0;                                        //��ʱ����������	
	while(time<timeout)                         //�ȴ���ʱ���
	{
			SIM800C_RX_STA=0;                              //SIM800C����״̬��ʶ����                            
	    memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);  //���SIM800C���ڽ��ջ����� 
	    SIM800C_printf("AT+CREG?\r\n");                //����ָ��
			delay_ms(100);                                //��ʱ1s
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));	
		if(strstr( (const char *)SIM800C_RX_BUF , (const char *)"+CREG: 0,1"))//������յ�+CREG:0,1ע������ɹ�
		{
			UartWriteBuffer_Config(SIM800C_RX_BUF,strlen((char *)SIM800C_RX_BUF));		
			break;       //����whileѭ��
		}
		time++;          //��ʱ������+1
	}
	if(time>=timeout)return 1;   //��ʱ���󣬷���1
	else return 0;		         //�޴�����0
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
