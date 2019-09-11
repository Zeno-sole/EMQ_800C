#include "bsp.h"

uint16_t HeartbeatCount = 0;

/**
 * @brief  发送AT命令
 * @param  无
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
 */
uint8_t Send_AT_Command(char *cmd, char *reply, uint32_t waittime){
	
	GPRS_CLEAN_RX();             //清空了接收缓冲区数据
	
	GPRS_TX(cmd);                //发送命令
	
	GPRS_DEBUG("Send AT Command:%s\r\n",cmd);
	
	if(reply == 0){  //不需要接收数据
		return GPRS_TRUE;
	}

	GPRS_DELAY(waittime); //延时

	return Check_ACK(reply);  //对接收数据进行处理
}


/**
 * @brief  检查ACK
 * @param  无
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
 */
uint8_t Check_ACK(char *reply){
	
	uint16_t len;
	uint8_t n;
	uint8_t off;
	char *redata;

	redata = GPRS_RX(len);   //接收数据

	*(redata+len) = '\0';

	GPRS_DEBUG("Reply Ack:%s\r\n",redata);

	n = 0;
	off = 0;
	
	while((n + off)<len){
		
		if(reply[n] == 0){  //数据为空或者比较完毕
			return GPRS_TRUE;
		}

		if(redata[ n + off]== reply[n]){
			n++; //移动到下一个接收数据
		}else{
			off++; //进行下一轮匹配
			n=0;  //重来
		}
		//n++;
	}

	if(reply[n]==0){ //刚好匹配完毕
		return GPRS_TRUE;
	}

	return GPRS_FALSE;  //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

/**
 * @brief  GPRS连网
 * @param  无
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
 */
void GPRS_Connect(void){
	
	char temp[50] = {0};
	uint16_t counts = 0;
	mySensorDatas.GPRS_FLAG=0;
	/*----------------------------------------------------------------------*/
	counts = 10;
	while((GPRS_Init()!= GPRS_TRUE) && (counts > 0)){ //检测模块响应是否正常,检测10次
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
	if((IsInsertCard() != GPRS_TRUE) && (counts > 0)){ //检查SIM卡是否正常
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
	GPRS_LinkClose();//断开之前的IP链接
	GPRS_DELAY(100);
	GPRS_ShutClose();//关闭之前的场景

  /*----------------------------------------------------------------------*/
	counts = 5;
	if((GSM_GPRS_init() != GPRS_TRUE) && (counts > 0)){ //重新初始化GPRS环境
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
		GPRS_LinkClose();//断开之前的IP链接
		GPRS_DELAY(100);
		GPRS_ShutClose();//关闭之前的场景
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
		LINK_LED_PORT->ODR |= (LINK_LED_PIN);//亮灯
		USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, ENABLE);//空闲中断
		uart_buffValid = 0;
		HeartbeatCount = 0;
		GPRS_CLEAN_RX();
		printf("GPRS init OK! Register device ID OK\r\n");
		mySensorDatas.GPRS_FLAG=1;
	}
}

/**
 * @brief  初始化并检测模块
 * @param  无
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
 */
uint8_t GPRS_Init(void){
	
	char *redata;
	uint16_t len;
	
	GPRS_CLEAN_RX();     //清空了接收缓冲区数据
//	Send_AT_Command("AT+CGMM\r\n", "OK", 500);
//	if(strstr( (const char *)uart_buff , (const char *)"OK"))//如果接收到OK表示指令成功
//		{
//			redata = GPRS_RX(len);   //接收数据
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
	
			redata = GPRS_RX(len);   //接收数据
			printf("redata= %s\r\n",redata);
	if(len == 0){
		return GPRS_FALSE;
	}
	//本程序兼容GSM900A、SIM800A、SIM800C
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
 * @brief  检测是否有卡
 * @param  无
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
 */
uint8_t IsInsertCard(void){
	
	GPRS_CLEAN_RX();
	return Send_AT_Command("AT+CNUM\r\n", "OK", 500);
}

/**
 * @brief  初始化GPRS网络
 * @param  无
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
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
 * @brief  建立TCP连接，最长等待时间20秒，可自行根据需求修改
	* @param  localport: 本地端口
	* @param  serverip: 服务器IP
	* @param  serverport: 服务器端口
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
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
	
	//检测是否建立连接
	while(Check_ACK("CONNECT OK") != GPRS_TRUE){		
		if(++testConnect > 500){ //最长等待20秒
			return GPRS_FALSE;
		}
		GPRS_DELAY(100);
	}
	return GPRS_TRUE;
}

/**
 * @brief  建立UDP连接，最长等待时间20秒，可自行根据需求修改
	* @param  localport: 本地端口
	* @param  serverip: 服务器IP
	* @param  serverport: 服务器端口
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
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
	
		//检测是否建立连接
	while(Check_ACK("CONNECT OK") != GPRS_TRUE ){
		
		if(++testConnect >200){ //最长等待20秒
			return GPRS_FALSE;
		}
		GPRS_DELAY(100); 		
	}		
		
	return GPRS_TRUE;
}

/**
 * @brief  使用GPRS发送数据，发送前需要先建立UDP或TCP连接
	* @param  str: 要发送的数据
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
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
		
		//检测是否发送完成
		while(Check_ACK("SEND OK") != GPRS_TRUE){	
			
			if(++testSend >200){ //最长等待20秒
				goto gprs_send_failure;
			}
			GPRS_DELAY(100); 		
		}
		return GPRS_TRUE;
	}else{
		
gprs_send_failure:
		
		end = 0x1B;
		Send_AT_Command(&end, 0, 0);	//ESC,取消发送 

		return GPRS_FALSE;
	}
}

/**
 * @brief  断开网络连接
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
 */
uint8_t GPRS_LinkClose(void){
	
	GPRS_CLEAN_RX();
	if(Send_AT_Command("AT+CIPCLOSE=1\r\n", "OK", 200) != GPRS_TRUE){
		return GPRS_FALSE;
	}
	return GPRS_TRUE;
}

/**
 * @brief  关闭场景
 * @retval 失败GPRS_FALSE  成功GPRS_TRUE
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
 * @brief  GPRS开机
 * @retval 无
 */
void GPRS_PWR(uint16_t sec){
	
	GPRS_PWR_PORT->ODR |= (GPRS_PWR_PIN);//Set GPRS PWR Pin Low
	BSP_OS_TimeDly(sec*1000);
	GPRS_PWR_PORT->ODR &= ~(GPRS_PWR_PIN);//Set GPRS PWR Pin High
	BSP_OS_TimeDly(1000);
}

/**
 * @brief  GPRS轮询数据
 * @retval 无
 */
void GPRS_Poll(void){
	
	if(uart_buffValid == 1){
		uart_buffValid = 0;
		uart_p = 0;
		char temp[32];
		memset(temp, 0, sizeof(temp));
		sprintf(temp, "*3$SHEB%d$HTBT$#",  DEVICE_ID);//心跳包格式
		if(memcmp((char*)uart_buff, temp, strlen(temp)) == 0){
			HeartbeatCount = 0;//收到的心跳包清零
			printf("Heartbeat ok.\r\n");
			LINK_LED_PORT->ODR |= (LINK_LED_PIN);//亮灯
			mySensorDatas.GPRS_FLAG = 1;
		}
			else if((strstr( (const char *)uart_buff , (const char *)"CLOSED"))){
	
		mySensorDatas.GPRS_FLAG = 0;
	
	}
		memset(uart_buff, 0, sizeof(uart_buff));
	}
}

/**
 * @brief  判断心跳包
 * @retval 无
 */
void HeartPack(void){
	//超过5分钟HeartbeatCount不清零表示心跳包丢失，将重新连接
	if(++HeartbeatCount >= 3000){
		printf("Heartbeat timeout, GPRS reconnect!\r\n");
		USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, DISABLE);//空闲中断
		uart_buffValid = 0;
		LINK_LED_PORT->ODR &= ~(LINK_LED_PIN);//灯灭
		printf("Wait the module reset...\r\n");
		GPRS_PWR(35);//重启
		printf("GPRS start init...\r\n");
		GPRS_Connect();//重新连接
		HeartbeatCount = 0;
	}
	else if((strstr( (const char *)uart_buff , (const char *)"CLOSED"))){
	
		mySensorDatas.GPRS_FLAG = 0;
	
	}
}

/**
 * @brief  上传传感器数据
 * @retval 无
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
			USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, DISABLE);//注意:发送数据之前先禁止空闲中断
//			GPRS_Send(temp);
			if(GPRS_Send(temp) == GPRS_FALSE)
			{
				
				mySensorDatas.GPRS_FLAG = 0;
			
			}
			uart_buffValid = 0;
			USART_ITConfig(GPRS_USARTx, USART_IT_IDLE, ENABLE);//注意:发送完成后启用空闲中断
		}
}
