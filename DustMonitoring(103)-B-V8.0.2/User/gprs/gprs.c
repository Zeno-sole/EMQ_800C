#include "bsp.h"

uint16_t HeartbeatCount = 0;

/**
 * @brief  ����AT����
 * @param  ��
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t Send_AT_Command(char *cmd, char *reply, uint32_t waittime){
	
	GPRS_CLEAN_RX();             //����˽��ջ���������
	
	GPRS_TX(cmd);                //��������
	
	GPRS_DEBUG("Send AT Command:%s\r\n",cmd);
	
	if(reply == 0){  //����Ҫ��������
		return GPRS_TRUE;
	}

	GPRS_DELAY(waittime); //��ʱ

	return Check_ACK(reply);  //�Խ������ݽ��д���
}


/**
 * @brief  ���ACK
 * @param  ��
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t Check_ACK(char *reply){
	
	uint16_t len;
	uint8_t n;
	uint8_t off;
	char *redata;

	redata = GPRS_RX(len);   //��������

	*(redata+len) = '\0';

	GPRS_DEBUG("Reply Ack:%s\r\n",redata);

	n = 0;
	off = 0;
	
	while((n + off)<len){
		
		if(reply[n] == 0){  //����Ϊ�ջ��߱Ƚ����
			return GPRS_TRUE;
		}

		if(redata[ n + off]== reply[n]){
			n++; //�ƶ�����һ����������
		}else{
			off++; //������һ��ƥ��
			n=0;  //����
		}
		//n++;
	}

	if(reply[n]==0){ //�պ�ƥ�����
		return GPRS_TRUE;
	}

	return GPRS_FALSE;  //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
}

/**
 * @brief  GPRS����
 * @param  ��
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
void GPRS_Connect(void){
	
	char temp[50] = {0};
	uint16_t counts = 0;
	mySensorDatas.GPRS_FLAG=0;
	/*----------------------------------------------------------------------*/
	counts = 10;
	while((GPRS_Init()!= GPRS_TRUE) && (counts > 0)){ //���ģ����Ӧ�Ƿ�����,���10��
		counts--;
		printf("GPRS Module Test %d!\r\n", counts);
		GPRS_DELAY(1000);
	}
	
	if(counts == 0){
		printf("GPRS Module init is No response!\r\n");
		return;
	}
	/*----------------------------------------------------------------------*/
	counts = 5;
	if((IsInsertCard() != GPRS_TRUE) && (counts > 0)){ //���SIM���Ƿ�����
		counts--;
		printf("SIM Card Test %d!\r\n", counts);
		GPRS_DELAY(1000);
	}
	
	if(counts == 0){
		printf("No SIM card detected, please power off and re-insert the SIM card!!!\r\n");
		return;
	}
	/*----------------------------------------------------------------------*/
  GPRS_DELAY(100);
	GPRS_LinkClose();//�Ͽ�֮ǰ��IP����
	GPRS_DELAY(100);
	GPRS_ShutClose();//�ر�֮ǰ�ĳ���

  /*----------------------------------------------------------------------*/
	counts = 5;
	if((GSM_GPRS_init() != GPRS_TRUE) && (counts > 0)){ //���³�ʼ��GPRS����
		counts--;
		printf("GPRS environment init %d\r\n", counts);
		GPRS_DELAY(1000);
	}
	
	if(counts == 0){
		printf("GPRS environment init failed\r\n");
		return;
	}
	/*----------------------------------------------------------------------*/
	GPRS_DELAY(100);
		counts = 10;
//	if(GPRS_TCPLink("2000", "180.76.135.14", "6000") != GPRS_TRUE)
		while((GPRS_TCPLink("2000", "180.76.135.14", "6000") != GPRS_TRUE) && (counts > 0))
		{
		printf("TCP connection failed!\r\n");
		GPRS_DELAY(100);
		GPRS_LinkClose();//�Ͽ�֮ǰ��IP����
		GPRS_DELAY(100);
		GPRS_ShutClose();//�ر�֮ǰ�ĳ���
		counts--;
		if(counts == 1)
		{
			break;
		}
//		return;
	}
	
	GPRS_DELAY(100);
	
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "*0$SHEB%d$1234#", DEVICE_ID);
	
	if(GPRS_Send(temp) == GPRS_TRUE){
		LINK_LED_PORT->ODR |= (LINK_LED_PIN);//����
		USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, ENABLE);//�����ж�
		uart_buffValid = 0;
		HeartbeatCount = 0;
		GPRS_CLEAN_RX();
		printf("GPRS init OK! Register device ID OK\r\n");
		mySensorDatas.GPRS_FLAG=1;
	}
}

/**
 * @brief  ��ʼ�������ģ��
 * @param  ��
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t GPRS_Init(void){
	
	char *redata;
	uint16_t len;
	
	GPRS_CLEAN_RX();     //����˽��ջ���������
//	Send_AT_Command("AT+CGMM\r\n", "OK", 500);
//	if(strstr( (const char *)uart_buff , (const char *)"OK"))//������յ�OK��ʾָ��ɹ�
//		{
//			redata = GPRS_RX(len);   //��������
//			printf("redata= %s\r\n",redata);
//			GPRS_CLEAN_RX();
//			return GPRS_TRUE;
//		}
//  else
//	{
//		
//		return GPRS_FALSE;
//	
//	}
		
	if(Send_AT_Command("AT+CGMM\r\n", "OK", 500) != GPRS_TRUE){
		printf("restart  error\r\n");
		return GPRS_FALSE;
	}
	
	else printf("restart  ok\r\n");
	
			redata = GPRS_RX(len);   //��������
			printf("redata= %s\r\n",redata);
	if(len == 0){
		return GPRS_FALSE;
	}
	//���������GSM900A��SIM800A��SIM800C
	if (strstr(redata,"SIMCOM_GSM900A") != 0){
		return GPRS_TRUE;
	}else if(strstr(redata,"SIMCOM_SIM800") != 0){
		return GPRS_TRUE;
	}else if(strstr(redata,"SIMCOM_SIM800C") != 0){
		return GPRS_TRUE;
	}else{
		return GPRS_FALSE;	
	}

}

/**
 * @brief  ����Ƿ��п�
 * @param  ��
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t IsInsertCard(void){
	
	GPRS_CLEAN_RX();
	return Send_AT_Command("AT+CNUM\r\n", "OK", 500);
}

/**
 * @brief  ��ʼ��GPRS����
 * @param  ��
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t GSM_GPRS_init(void){
	
	GPRS_CLEAN_RX();
	if(Send_AT_Command("AT+CGCLASS=\"B\"\r\n","OK", 1000) != GPRS_TRUE) return GPRS_FALSE;

	GPRS_CLEAN_RX();
	if(Send_AT_Command("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK", 1000) != GPRS_TRUE) return GPRS_FALSE;		

	GPRS_CLEAN_RX();	
	if(Send_AT_Command("AT+CGATT=1\r\n","OK", 1000)!= GPRS_TRUE) return GPRS_FALSE;

	GPRS_CLEAN_RX();
	if(Send_AT_Command("AT+CIPCSGP=1,\"CMNET\"\r\n","OK", 1000)!= GPRS_TRUE) return GPRS_FALSE;
	return GPRS_TRUE;
}

/**
 * @brief  ����TCP���ӣ���ȴ�ʱ��20�룬�����и��������޸�
	* @param  localport: ���ض˿�
	* @param  serverip: ������IP
	* @param  serverport: �������˿�
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t GPRS_TCPLink(char *localport, char * serverip, char * serverport){
	
	char cmd_buf[100];
	uint16_t testConnect=0;
		
	sprintf(cmd_buf,"AT+CLPORT=\"TCP\",\"%s\"\r\n",localport);
	
	if(Send_AT_Command(cmd_buf,"OK", 1000)!= GPRS_TRUE)
		return GPRS_FALSE;
	
	
	GPRS_CLEAN_RX();
	sprintf(cmd_buf,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",serverip,serverport);	
	Send_AT_Command(cmd_buf,0, 1000);	
	
	//����Ƿ�������
	while(Check_ACK("CONNECT OK") != GPRS_TRUE){		
		if(++testConnect > 500){ //��ȴ�20��
			return GPRS_FALSE;
		}
		GPRS_DELAY(100);
	}
	return GPRS_TRUE;
}

/**
 * @brief  ����UDP���ӣ���ȴ�ʱ��20�룬�����и��������޸�
	* @param  localport: ���ض˿�
	* @param  serverip: ������IP
	* @param  serverport: �������˿�
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t GPRS_UDPLink(char *localport,char * serverip,char * serverport){
	
	char cmd_buf[100];
	uint8_t testConnect=0;

	sprintf(cmd_buf,"AT+CLPORT=\"UDP\",\"%s\"\r",localport);
	
	if(Send_AT_Command(cmd_buf,"OK", 100)!= GPRS_TRUE)
		return GPRS_FALSE;
	
	GPRS_CLEAN_RX();
		
	sprintf(cmd_buf,"AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);
	
	Send_AT_Command(cmd_buf,0, 100);	
	
		//����Ƿ�������
	while(Check_ACK("CONNECT OK") != GPRS_TRUE ){
		
		if(++testConnect >200){ //��ȴ�20��
			return GPRS_FALSE;
		}
		GPRS_DELAY(100); 		
	}		
		
	return GPRS_TRUE;
}

/**
 * @brief  ʹ��GPRS�������ݣ�����ǰ��Ҫ�Ƚ���UDP��TCP����
	* @param  str: Ҫ���͵�����
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t GPRS_Send(const char * str)
{
	char end = 0x1A;
	uint8_t testSend=0;

	GPRS_CLEAN_RX();
	
	if(Send_AT_Command("AT+CIPSEND\r\n", ">", 500) == 0){
	
		GPRS_USART_printf("%s",str);
		//printf("Send String:%s\r\n", str);

		GPRS_CLEAN_RX();
		Send_AT_Command(&end, 0, 100);		
		
		//����Ƿ������
		while(Check_ACK("SEND OK") != GPRS_TRUE){	
			
			if(++testSend >200){ //��ȴ�20��
				goto gprs_send_failure;
			}
			GPRS_DELAY(100); 		
		}
		return GPRS_TRUE;
	}else{
		
gprs_send_failure:
		
		end = 0x1B;
		Send_AT_Command(&end, 0, 0);	//ESC,ȡ������ 

		return GPRS_FALSE;
	}
}

/**
 * @brief  �Ͽ���������
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t GPRS_LinkClose(void){
	
	GPRS_CLEAN_RX();
	if(Send_AT_Command("AT+CIPCLOSE=1\r\n", "OK", 200) != GPRS_TRUE){
		return GPRS_FALSE;
	}
	return GPRS_TRUE;
}

/**
 * @brief  �رճ���
 * @retval ʧ��GPRS_FALSE  �ɹ�GPRS_TRUE
 */
uint8_t GPRS_ShutClose(void){
	
	uint8_t  check_time=0;
	GPRS_CLEAN_RX();
	
	Send_AT_Command("AT+CIPSHUT\r\n", 0, 0);
	
	while(Check_ACK("OK") != GPRS_TRUE){
		
		if(++check_time >50)return GPRS_FALSE;
		
		GPRS_DELAY(200);
	}

	return GPRS_TRUE;
}

/**
 * @brief  GPRS����
 * @retval ��
 */
void GPRS_PWR(uint16_t sec){
	
	GPRS_PWR_PORT->ODR |= (GPRS_PWR_PIN);//Set GPRS PWR Pin Low
	BSP_OS_TimeDly(sec*1000);
	GPRS_PWR_PORT->ODR &= ~(GPRS_PWR_PIN);//Set GPRS PWR Pin High
	BSP_OS_TimeDly(1000);
}

/**
 * @brief  GPRS��ѯ����
 * @retval ��
 */
void GPRS_Poll(void){
	
	if(uart_buffValid == 1){
		uart_buffValid = 0;
		uart_p = 0;
		char temp[32];
		memset(temp, 0, sizeof(temp));
		sprintf(temp, "*3$SHEB%d$HTBT$#",  DEVICE_ID);//��������ʽ
		if(memcmp((char*)uart_buff, temp, strlen(temp)) == 0){
			HeartbeatCount = 0;//�յ�������������
			printf("Heartbeat ok.\r\n");
			LINK_LED_PORT->ODR |= (LINK_LED_PIN);//����
			mySensorDatas.GPRS_FLAG = 1;
		}
			else if((strstr( (const char *)uart_buff , (const char *)"CLOSED"))){
	
		mySensorDatas.GPRS_FLAG = 0;
	
	}
		memset(uart_buff, 0, sizeof(uart_buff));
	}
}

/**
 * @brief  �ж�������
 * @retval ��
 */
void HeartPack(void){
	//����5����HeartbeatCount�������ʾ��������ʧ������������
	if(++HeartbeatCount >= 3000){
		printf("Heartbeat timeout, GPRS reconnect!\r\n");
		USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, DISABLE);//�����ж�
		uart_buffValid = 0;
		LINK_LED_PORT->ODR &= ~(LINK_LED_PIN);//����
		printf("Wait the module reset...\r\n");
		GPRS_PWR(35);//����
		printf("GPRS start init...\r\n");
		GPRS_Connect();//��������
		HeartbeatCount = 0;
	}
	else if((strstr( (const char *)uart_buff , (const char *)"CLOSED"))){
	
		mySensorDatas.GPRS_FLAG = 0;
	
	}
}

/**
 * @brief  �ϴ�����������
 * @retval ��
 */
void UploadData(void){
	char temp[100] = {0};
	BSP_OS_TimeDly(8500*6);
	sprintf(temp, "*1$SHEB%d$D1$%d$D2$%d$D3$%d$D4$%d$D5$%d$D6$%d$D7$%d$D8$%d$D9$%d$D10$%d$",
											DEVICE_ID,
											mySensorDatas.PM2_5,//D1
											mySensorDatas.PM10,//D2
											mySensorDatas.NOISE,//D3
											mySensorDatas.TEMP/10,//D4
											mySensorDatas.HUMI/10,//D5
											mySensorDatas.WIND_SPEED,//D6
											mySensorDatas.WIND_DIRECTION,//D7
											mySensorDatas.TSP,//D8
											mySensorDatas.WIND_LEVEL,//D9
											0);
		if(mySensorDatas.GPRS_FLAG)
		{
			USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, DISABLE);//ע��:��������֮ǰ�Ƚ�ֹ�����ж�
//			GPRS_Send(temp);
			if(GPRS_Send(temp) == GPRS_FALSE)
			{
				
				mySensorDatas.GPRS_FLAG = 0;
			
			}
			uart_buffValid = 0;
			USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, ENABLE);//ע��:������ɺ����ÿ����ж�
		}
}
