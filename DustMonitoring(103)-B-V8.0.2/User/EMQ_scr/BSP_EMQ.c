#include "BSP_EMQ.h"


u8 Transmission_Sta;           //1����ʾָ��״̬    0����ʾ͸��״̬
u8 serverLink_sta=0;           //0��δ���ӷ�����    1�������Ϸ�����
u8 ping_flag=0;                //1��������ping����  0���յ��������ظ�
u8 EMQ_RES=0;                //1
u8 EMQ_success=0;                //1
u8 res;
u8 temp_buff[128];
u8 relay_flag = 3;
char *Cntr_CMD = "test";    //APP���ƶ˵�topic����        ���ĵ�����          
char *Deal_CMD = "test1";    //������ִ�ж˵�topic����			����������
char  Cntr_CMD_t[50] = "test";    //APP���ƶ˵�topic����        ���ĵ�����  
char  Deal_CMD_t[50] = "test1";    //������ִ�ж˵�topic����			����������
char *CMD1 = "y";          //   
char *CMD2 = "g";          //�ػ��ź�   
char *CMD3 = "k";          //�����ź�
char *CMD4 = "AT+SWITCH1=1";     //��һ·���ؿ�������D2
char *CMD5 = "AT+SWITCH1=0";     //��һ·���عأ�Ϩ��D2

uint32_t falsh_DEVICE_ID[2];	//write flash ��������
uint32_t falsh_DEVICE_id[2];
uint32_t DEVICE_ID;
uint32_t DEVICE_IDc;
uint8_t  RxCmdBuff[128];   //����������ջ�����
uint16_t RxCmdCount;      //�������ݼ�����


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
	
	SIM800C_init();             //SIM800C���ڳ�IO
 
	SIM800CUsart_init(115200);  //SIM800C���ڳ�ʼ��������115200


}
void EMQ_POLL(void){
//	OS_ERR err;
    char temp_sec[50];
/*--------------------------------------------------------------------------------------------*/
		/*                            �����Ϸ������󣬿�ʼ����MQTT��Ӧ��                              */
	    /*--------------------------------------------------------------------------------------------*/

		if(serverLink_sta==1)    
		{										  		
			/*--------------------------------------------------------------------------------------------*/
			/*                                      ����������                                          */
			/*--------------------------------------------------------------------------------------------*/
			if(EMQTT_TxDataOutPtr != EMQTT_TxDataInPtr)        //if�����Ļ���˵�����ͻ�������������
			{
				printf("��������\r\n");
				EMQTT_TxData();                                //��������
				EMQTT_TxDataOutPtr+=EMQTT_UNIT;                //ָ������
				if(EMQTT_TxDataOutPtr==EMQTT_TxDataEndPtr)     //���ָ�뵽������β����
					EMQTT_TxDataOutPtr = EMQTT_TxDataBuf[0];   //ָ���λ����������ͷ				
			}	
			/*--------------------------------------------------------------------------------------------*/
			/*                                      �����������                                          */
			/*--------------------------------------------------------------------------------------------*/
			else if(EMQTT_RxDataOutPtr != EMQTT_RxDataInPtr)        //if�����Ļ���˵�����ջ�������������
			{				
				
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/
				if(EMQTT_RxDataOutPtr[1]==0x20)                //0x20��ͷ�����ǣ�ȷ������������
				{
				    if(EMQTT_CONNACK(EMQTT_RxDataOutPtr))      //����ñ���,����ֵ��0����ʾ����
					{
						printf("CONNECT������ʧ�ܣ�׼������\r\n"); //������ʾ��Ϣ
						NVIC_SystemReset();                        //����
					}
					else
					{
						printf("CONNECT�������ɹ�\r\n");           //������ʾ��Ϣ
						if(EMQTT_SUBSCRIBE(Cntr_CMD,0)!=0)
						{
							printf("SUBSCRIBE����topic���Ĵ���\r\n");
						}
			
						ping_flag=0;                               //ping��־����
						TIM4_Init();                               //30����ping
						TIM5_Init();                               //8��ping��ʱ���			
						TIM2_Init();
						
					}					
				}
				
				/*-----------------------------------------------------*/
				/*            ���������������PUBLISH_Qs0����          */
				/*-----------------------------------------------------*/
				else if(EMQTT_RxDataOutPtr[1]==0x30)                //0x30��ͷ�����ǣ��ȼ�0��PUBLISH����
				{
				    if(EMQTT_Deal_Qs0(EMQTT_RxDataOutPtr))          //����ñ���,����0����ʾ��ȷ
					{
						printf("�ǿ��ƶ˱��ģ���������\r\n");
					}				
				}				
				
				/*-----------------------------------------------------*/
				/*                    ����PINGRESP����                 */
				/*-----------------------------------------------------*/				
				else if(EMQTT_RxDataOutPtr[1]==0xD0)                //0xD0��ͷ�����ǣ�������Ӧ����
				{
					if(EMQTT_PINGRESP(EMQTT_RxDataOutPtr))          //����ñ���,����ֵ��0����ʾ����
					{
						printf("ping��Ӧ����׼����������\r\n");   //������ʾ��Ϣ
					}
					else
					{
						printf("�յ�ping��Ӧ\r\n");  
						ping_flag=0;                 //�����־
						TIM_Cmd(TIM5, DISABLE);      //�رն�ʱ��5	
						TIM_SetCounter(TIM5, 0);     //���㶨ʱ��5������										
					}
				}
				
				/*-----------------------------------------------------*/
				/*                    ����SUBACK����                   */
				/*-----------------------------------------------------*/
				else if(EMQTT_RxDataOutPtr[1]==0x90)           //0x90��ͷ�����ǣ����Ļظ�����
				{
				    res=EMQTT_SUBACK(EMQTT_RxDataOutPtr);      //����ñ���,����ֵ��0����ʾ����
					
					switch(res)
					{
						case 0 : 
									EMQ_success = 1;
									TIM_Cmd(TIM2,ENABLE); 

									sprintf(temp_sec,"����topic�ɹ�\r\n");
									Uart5WriteBuffer_Config((u8 *)temp_sec, strlen(temp_sec));
									LINK_LED_PORT->ODR |= (LINK_LED_PIN);//����
									memset(temp_sec, 0, sizeof(temp_sec));
				
								 break;
						case 1 : printf("����topicʧ�ܣ�׼������\r\n");
								 delay_ms(500);
								 NVIC_SystemReset(); //����
								 break;
						
						case 2 : printf("�ظ�����׼������\r\n");
								 delay_ms(500);
								 NVIC_SystemReset(); //����
								 break;
						
						case 3 : printf("�ظ�����׼������\r\n");
								 delay_ms(500);
								 NVIC_SystemReset(); //����
								 break;
					}					
				}
												
				/*-----------------------------------------------------*/
				/*                      ����δ֪����                   */
				/*-----------------------------------------------------*/
				else
				{
					printf("δ֪����\r\n");		
//					NVIC_SystemReset(); //����					
				}								
				
				/*----------------������һ�����ݺ�ָ�����ƣ�һ��ע�ⲻҪ����˳��------------*/
				/*------------------------һ��ע����if else if�ṹ----------------------------*/
				EMQTT_RxDataOutPtr+=EMQTT_UNIT;                //ָ������
				if(EMQTT_RxDataOutPtr==EMQTT_RxDataEndPtr)     //���ָ�뵽������β����
					EMQTT_RxDataOutPtr = EMQTT_RxDataBuf[0];   //ָ���λ����������ͷ
			}
			
			/*--------------------------------------------------------------------------------------------*/
			/*                                  ������Ƶ���������                                        */
			/*--------------------------------------------------------------------------------------------*/
			else if(EMQTT_CMDOutPtr != EMQTT_CMDInPtr)                 //if�����Ļ���˵�����������������
			{
				printf("��������:%s\r\n",&EMQTT_CMDOutPtr[1]);

				if(!memcmp(&EMQTT_CMDOutPtr[1],CMD1,strlen(CMD1)))     //�ж�ָ������CMD1���Ϳ�ʼ�ɼ���ʪ��
				{   

					  printf("�յ���ѯָ��\r\n");
					  memset(temp_buff,0,128);
						sprintf((char *)temp_buff,"ok");          //�����ϴ�����			
						EMQTT_PUBLISH_Qs0(Deal_CMD,temp_buff);                            //�������� 		
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD2,strlen(CMD2))) //�ж�ָ������CMD2���Ϳ�ʼ�ɼ���ǿ��
				{   
					printf("�յ��ػ�ָ��\r\n");
					memset(temp_buff,0,128);
					sprintf((char *)temp_buff,"close");                  //�����ϴ�����
					EMQTT_PUBLISH_Qs0(Deal_CMD,temp_buff);                           //�������� 
					relay_flag = 2;					
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD3,strlen(CMD3))) //�ж�ָ������CMD3���Ϳ�ʼ�ɼ�����ѹ
				{   
					  printf("�յ�����ָ��\r\n");
					  memset(temp_buff,0,128);
						sprintf((char *)temp_buff,"open");          //�����ϴ�����			
						EMQTT_PUBLISH_Qs0(Deal_CMD,temp_buff);                            //�������� 
						relay_flag = 1;
						
					
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD4,strlen(CMD4))) //�ж�ָ������CMD4������D2
				{                     
					printf("����D2\r\n");
//					LED0=0;
//					LED1 = 1;
					EMQTT_PUBLISH_Qs0(Deal_CMD,"+SW1:1");  //�ظ�����aPP���򿪿�����
				}
				else if(!memcmp(&EMQTT_CMDOutPtr[1],CMD5,strlen(CMD5))) //�ж�ָ������CMD5��Ϩ��D2
				{                     
					printf("Ϩ��D2\r\n");
//					LED0=1;
//					LED1 = 0;
					EMQTT_PUBLISH_Qs0(Deal_CMD,"+SW1:0");  //�ظ�����aPP���򿪿�����
				}
				
				else
				{
				 EMQTT_RxDataOutPtr = EMQTT_RxDataBuf[0];   //ָ���λ����������ͷ
					printf("δ֪����׼������\r\n");
					NVIC_SystemReset();                            //����
				}
								
				EMQTT_CMDOutPtr+=EMQTT_UNIT;             //ָ������
				if(EMQTT_CMDOutPtr==EMQTT_CMDEndPtr)     //���ָ�뵽������β����
					EMQTT_CMDOutPtr = EMQTT_CMDBuf[0];   //ָ���λ����������ͷ				
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
 char write_config_cmd[] = "rite:"; //д����
 char read_config_cmd[] = "read";   //������
		  if(Find_usart5(write_config_cmd)){
//			OSTaskSuspend((OS_TCB*)&AppTaskCollectionTCB,&os_err);//��������2
			i = StringFind((char *)RxCmdBuff,write_config_cmd)+4;
			printf("i=%d\r\n",i);
		 if((RxCmdBuff[i+1] != 0X00)&&(RxCmdBuff[i+2] != 0X00)&&(RxCmdBuff[i+3] != 0X00)&&(RxCmdBuff[i+4] != 0X00)&&(RxCmdBuff[i+5] != 0X00)&&(RxCmdBuff[i+6] != 0X00))
			{
				DEVICE_ID = ((RxCmdBuff[i+1]-0X30)*100000)+((RxCmdBuff[i+2]-0X30)*10000)+((RxCmdBuff[i+3]-0X30)*1000)+((RxCmdBuff[i+4]-0X30)*100)+((RxCmdBuff[i+5]-0X30)*10)+((RxCmdBuff[i+6]-0X30));
				
				sprintf(temp,"���д�������Ե�...\r\n");		
			  Uart5WriteBuffer_Config((u8 *)temp, strlen(temp));
				memset(temp, 0, sizeof(temp));
				write_DEVICE_ID();
				sprintf(temp,"д��ɹ�!���Ϊ:%d\r\n",DEVICE_ID);
			  Uart5WriteBuffer_Config((u8 *)temp, strlen(temp));
				memset(temp, 0, sizeof(temp));
//				OSTaskResume((OS_TCB*)&AppTaskCollectionTCB,&os_err);	
			}
			else 
			{
				sprintf(temp,"��ȡ��������������\r\n");	
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
		if(falsh_DEVICE_id[0] > 0XFFFFF)   //Flash������֮��ʹ��Ĭ�ϲ���   
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
 * ���� IWDG �ĳ�ʱʱ��
 * Tout = prv/40 * rlv (s)
 *      prv������[4,8,16,32,64,128,256]
 * prv:Ԥ��Ƶ��ֵ��ȡֵ���£�
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:Ԥ��Ƶ��ֵ��ȡֵ��ΧΪ��0-0XFFF
 * �������þ�����
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���
 */

void IWDG_Config(uint8_t prv ,uint16_t rlv)
{	
	// ʹ�� Ԥ��Ƶ�Ĵ���PR����װ�ؼĴ���RLR��д
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// ����Ԥ��Ƶ��ֵ
	IWDG_SetPrescaler( prv );
	
	// ������װ�ؼĴ���ֵ
	IWDG_SetReload( rlv );
	
	// ����װ�ؼĴ�����ֵ�ŵ���������
	IWDG_ReloadCounter();
	
	// ʹ�� IWDG
	IWDG_Enable();	
}

// ι��
void IWDG_Feed(void)
{
	// ����װ�ؼĴ�����ֵ�ŵ��������У�ι������ֹIWDG��λ
	// ����������ֵ����0��ʱ������ϵͳ��λ
	IWDG_ReloadCounter();
}
void EMQ_Connect(void){
	OS_ERR  os_err;
  u8 conts;
	IWDG_Config(IWDG_Prescaler_128 ,4095);    //13s
	printf("\r\nĿǰ���ڹػ�״̬��׼������\r\n");

	
/*-------------------------------------------------------------------------------------------------*/
	Transmission_Sta=1;                   //ָ��״̬
	for(res=0;res<3;res++)                //����Ƿ���ATָ��״̬
	{
		if(SIM800C_TestAT(1)==0)          //��⣬��ʱʱ��500ms
		{
			printf("����ָ��״̬\r\n");   //������ʾ����
			break;                        //��⵽����ATָ��״̬������forѭ��
		}	
	}
	if(res>=3)  printf("���ڷ�ָ��״̬\r\n");   
/*-------------------------------------------------------------------------------------------------*/
//	if(SIM800C_CPIN(20))                         //��ѯSIM��״̬����ʱʱ��5s
//	{
//		while(1)
//		{
//			printf("SIM��״̬�쳣�����飬���Ƿ�巴\r\n");
//			delay_ms(100);
//			NVIC_SystemReset();  //����	
//		}
//	}
//	printf("SIM��״̬����\r\n");
/*-------------------------------------------------------------------------------------------------*/	
	conts = 10;
	while(SIM800C_CSQ(800)&&(conts>0)){
	
		conts--;
		delay_ms(500);
		printf("CSQֵ����\r\n");
	
	}

	delay_ms(100);
	IWDG_Feed();

	printf("CSQ����\r\n");
///*-------------------------------------------------------------------------------------------------*/
//	if(SIM800C_SendCmd_OK("AT+CIPCLOSE=1",20))      //�Ͽ���������
//	{
//		printf("�Ͽ���������ʧ�ܣ�׼������\r\n");
//		delay_ms(500);
//		NVIC_SystemReset();  //����	
//	}
//	  printf("�Ͽ��������ӳɹ�\r\n");
///*-------------------------------------------------------------------------------------------------*/	
	if(SIM800C_CREG(800))                //�ȴ��������ź� 50s
	{
		printf("��ѯ�����źţ���ʱʧ�ܣ�׼������\r\n");
		delay_ms(500);
		EMQ_Connect();  //����		
	}
	printf("��ѯ�������ź�\r\n");
/*-------------------------------------------------------------------------------------------------*/	
	if(SIM800C_SendCmd_OK("ATE0",50))               //�رջ��ԣ���ʱʱ��5s
	{
		printf("�رջ���ʧ�ܣ�׼������\r\n");
		delay_ms(500);
		EMQ_Connect();  //����			
	}
	printf("�رջ��Գɹ�\r\n");
/*-------------------------------------------------------------------------------------------------*/	
	if(SIM800C_SendCmd_OK("AT+GSMBUSY=1",50))      //����GSM�ַ�������ʱʱ��5s
	{
		printf("���ý�ֹ�绰����ʧ�ܣ�׼������\r\n");
		delay_ms(500);
		EMQ_Connect();  //����	
	}
	printf("���ý�ֹ�绰����ɹ�\r\n");

//	/*-------------------------------------------------------------------------------------------------*/
	if(SIM800C_SendCmd_OK("AT+CIPSHUT",50))      //�رճ���
	{
		printf("�رճ���ʧ�ܣ�׼������\r\n");
		delay_ms(500);
		EMQ_Connect();  //����	
	}
	printf("�رճ����ɹ�\r\n");
///*-------------------------------------------------------------------------------------------------*/
	if(SIM800C_SendCmd_OK("AT+CIPMODE=0",50))      //���õ�����ģʽ����ʱʱ��5s
	{
		printf("���õ�����ģʽʧ�ܣ�׼������\r\n");
		delay_ms(500);
		EMQ_Connect();  //����	
	}
	printf("���õ�����ģʽ�ɹ�\r\n");
///*-------------------------------------------------------------------------------------------------*/		
	if(SIM800C_SendCmd_OK("AT+CIPMODE=1",50))      //����͸��ģʽ����ʱʱ��5s
	{
		printf("����͸��ģʽʧ�ܣ�׼������\r\n");
		delay_ms(500);
		EMQ_Connect();  //����	
	}
	printf("����͸��ģʽ�ɹ�\r\n");
/*-------------------------------------------------------------------------------------------------*/
	
	delay_ms(200);
	res=SIM800C_GPRS(50);  //��ʼGPRS�����ӷ���������ʱ30s
//	res = 0;
	switch(res)
	{
		case 0: printf("\r\n���ӷ������ɹ�\r\n");         //������ʾ��Ϣ	

				TIM3_Init();		                      //��ʱ��3��ʼ��
				Transmission_Sta=0;                       //����͸��ģʽ��
				SIM800C_RX_STA=0;                               //SIM800C����״̬��ʶ����                            
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);   //���SIM800C���ڽ��ջ�����
				EMQTT_Init();                                   //EMQTT��ʼ��
				serverLink_sta=1;                               //��־��λ
//				LED1=0;		                                //����D1����ʾ�����Ϸ�����
		    EMQTT_CONNECT();                                //�������ӱ���		

		    OSTaskSuspend((OS_TCB*)&AppTaskGET1TCB,&os_err);//����1ִ��5�κ��������2
				break;                                          //����
		  
		case 1: printf("��ѯ����GPRS��ʱ��׼������\r\n"); //������ʾ��Ϣ
				delay_ms(500);
				EMQ_Connect();                       //����
				break;                                    //����
		
		case 2: printf("��ʼ����ʱ��׼������\r\n");     //������ʾ��Ϣ
				delay_ms(500);
				EMQ_Connect();                       //����
				break;                                    //����
		
		case 3: printf("����GPRS���ӳ�ʱ��׼������\r\n"); //������ʾ��Ϣ
				delay_ms(500);
				EMQ_Connect();                       //����
				break;                                    //����
		
		case 4: printf("��ȡIP��ַ��ʱ��׼������\r\n");   //������ʾ��Ϣ
				delay_ms(500);
				EMQ_Connect();                       //����
				break;                                    //����
				
		case 5: printf("���ӷ�������ʱ��׼������\r\n");   //������ʾ��Ϣ
        delay_ms(500);
				EMQ_Connect();                       //����
				break;  							      //����
	}

}
void Tcp_Connect(void){
	OS_ERR  os_err;
	IWDG_Feed();
	if(SIM800C_SendCmd_OK("AT+CIPCLOSE=1",20))      //�Ͽ���������
	{
		printf("�Ͽ���������ʧ�ܣ�׼������\r\n");
		delay_ms(500);

	}
	else{
	  printf("�Ͽ��������ӳɹ�\r\n");
	}
	if(SIM800C_SendCmd_OK("AT+CIPSHUT",50))      //�رճ���
	{
		printf("�رճ���ʧ�ܣ�׼������\r\n");
		delay_ms(500);
	
	}
	else{
	printf("�رճ����ɹ�\r\n");
	}
		delay_ms(200);
	res=SIM800C_GPRS(50);  //��ʼGPRS�����ӷ���������ʱ30s
//	res = 0;
	switch(res)
	{
		case 0: printf("\r\n���ӷ������ɹ�\r\n");         //������ʾ��Ϣ	

				TIM3_Init();		                      //��ʱ��3��ʼ��
				Transmission_Sta=0;                       //����͸��ģʽ��
				SIM800C_RX_STA=0;                               //SIM800C����״̬��ʶ����                            
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);   //���SIM800C���ڽ��ջ�����
				EMQTT_Init();                                   //EMQTT��ʼ��
				serverLink_sta=1;                               //��־��λ
//				LED1=0;		                                //����D1����ʾ�����Ϸ�����
		    EMQTT_CONNECT();                                //�������ӱ���		
				TIM_Cmd(TIM4, ENABLE);   
		OSTaskSuspend((OS_TCB*)&AppTaskGET1TCB,&os_err);//����1ִ��5�κ��������2
				break;                                          //����
		  
		case 1: printf("��ѯ����GPRS��ʱ��׼������\r\n"); //������ʾ��Ϣ
				delay_ms(500);
//				EMQ_Connect();                       //����
				break;                                    //����
		
		case 2: printf("��ʼ����ʱ��׼������\r\n");     //������ʾ��Ϣ
				delay_ms(500);
//				EMQ_Connect();                       //����
				break;                                    //����
		
		case 3: printf("����GPRS���ӳ�ʱ��׼������\r\n"); //������ʾ��Ϣ
				delay_ms(500);
//				EMQ_Connect();                       //����
				break;                                    //����
		
		case 4: printf("��ȡIP��ַ��ʱ��׼������\r\n");   //������ʾ��Ϣ
				delay_ms(500);
//				EMQ_Connect();                       //����
				break;                                    //����
				
		case 5: printf("���ӷ�������ʱ��׼������\r\n");   //������ʾ��Ϣ
        delay_ms(500);
//				EMQ_Connect();                       //����
				break;  							      //����
	}
	

}
void ResConnect(void){
	OS_ERR  os_err;
	OSTaskResume((OS_TCB*)&AppTaskGET1TCB,&os_err);	

}
