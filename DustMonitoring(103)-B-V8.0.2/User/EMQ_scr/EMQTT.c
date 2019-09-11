/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "EMQTT.h" 
#include "sim800c.h"
#include "sim800c_usart.h"
//#include "usart1.h"
//#include "led.h"

//#define  CLIENT_ID           "518727330"                       //客户端ID (  设备ID  )  //使用UID代替STM32唯一标识码
char *CLIENT_ID = "518727330";
#define  CLIENT_ID_LEN       strlen(CLIENT_ID)                //长度

#define  USERNAME_ID         "217577"                         //用户名   (  产品ID  )
#define  USERNAME_ID_LEN     strlen(USERNAME_ID)              //长度

#define  PASSWARD            "kfbskd"                         //密码     ( 鉴权信息 )
#define  PASSWARD_LEN         strlen(PASSWARD)                //长度

u8  EMQTT_RxDataBuf[EMQTT_NUM][EMQTT_UNIT];       //数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
u8 *EMQTT_RxDataInPtr;                            //指向缓冲区存放数据的位置
u8 *EMQTT_RxDataOutPtr;                           //指向缓冲区读取数据的位置
u8 *EMQTT_RxDataEndPtr;                           //指向缓冲区结束的位置

u8  EMQTT_TxDataBuf[EMQTT_NUM][EMQTT_UNIT];       //数据的发送缓冲区,所有发往服务器的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
u8 *EMQTT_TxDataInPtr;                            //指向缓冲区存放数据的位置
u8 *EMQTT_TxDataOutPtr;                           //指向缓冲区读取数据的位置
u8 *EMQTT_TxDataEndPtr;                           //指向缓冲区结束的位置

u8  EMQTT_CMDBuf[CMD_NUM][EMQTT_UNIT];            //命令数据的接收缓冲区
u8 *EMQTT_CMDInPtr;                               //指向缓冲区存放数据的位置
u8 *EMQTT_CMDOutPtr;                              //指向缓冲区读取数据的位置
u8 *EMQTT_CMDEndPtr;                              //指向缓冲区结束的位置

u16            Message_ID=1;                      //报文标识符，每个报文都应该分配一个不同的未占用的标识符
MessageOBD     EMQTT_MessageOBD[MessageOBD_NUM];  //有报文标识符的报文的管理结构体
MessageOBD    *EMQTT_MessageOBDPtr;               //指向MessageOBD结构体链表头的指针
MessageOBD    *First_MessageOBDPtr;               //被占用的MessageOBD结构体链表头的指针



/*----------------------------------------------------------*/
/*函数名：EMQTT初始化                                       */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void EMQTT_Init(void)
{	
	u8 i;
	
	Message_ID = 1;                                  //报文标识符,回到1
	
	EMQTT_RxDataInPtr=EMQTT_RxDataBuf[0];            //指针归位
	EMQTT_RxDataOutPtr=EMQTT_RxDataInPtr;            //指针归位
    EMQTT_RxDataEndPtr=EMQTT_RxDataBuf[EMQTT_NUM-1]; //指针归位
	
	EMQTT_TxDataInPtr=EMQTT_TxDataBuf[0];            //指针归位
	EMQTT_TxDataOutPtr=EMQTT_TxDataInPtr;            //指针归位
	EMQTT_TxDataEndPtr=EMQTT_TxDataBuf[EMQTT_NUM-1]; //指针归位
	
	EMQTT_CMDInPtr=EMQTT_CMDBuf[0];                  //指针归位
	EMQTT_CMDOutPtr=EMQTT_CMDInPtr;                  //指针归位
	EMQTT_CMDEndPtr=EMQTT_CMDBuf[CMD_NUM-1];         //指针归位	
	
	for(i=0;i<(MessageOBD_NUM-1);i++)                //构建链表
	{ 
		EMQTT_MessageOBD[i].NextPtr = &EMQTT_MessageOBD[i+1];
	}
	EMQTT_MessageOBD[MessageOBD_NUM-1].NextPtr = NULL;
	EMQTT_MessageOBDPtr = &EMQTT_MessageOBD[0];
	First_MessageOBDPtr = NULL;
}

/*----------------------------------------------------------*/
/*函数名：连接服务器报文                                    */
/*参  数：will_sta：0 不添加遗嘱报文  1 添加遗嘱报文        */
/*参  数：will_topic：遗嘱topic名称                         */
/*参  数：will_content：遗嘱topic内容                       */
/*参  数：will_QoS：遗嘱topic服务等级                       */
/*返回值：0：成功  其他： 失败                              */
/*----------------------------------------------------------*/
u8 EMQTT_CONNECT(void)
{
    u16 Fixed_len;              //固定报头长度
	u16 Variable_len;           //可变报头长度
	u16 Payload_len;            //有效负荷长度
    u8  temp_buff[EMQTT_UNIT];	//临时缓冲区

	Fixed_len = 2;                 //连接报文中，固定报头长度=2
	Variable_len = 10;             //连接报文中，可变报头长度=10
	Payload_len = 2 + CLIENT_ID_LEN + 2 + USERNAME_ID_LEN + 2 + PASSWARD_LEN;        
	
	temp_buff[0]=0x10;                            
	temp_buff[1]=Variable_len + Payload_len;
	
	temp_buff[2]=0x00;
	temp_buff[3]=0x04;
	temp_buff[4]=0x4D;
	temp_buff[5]=0x51;
	temp_buff[6]=0x54;
	temp_buff[7]=0x54;
	temp_buff[8]=0x04;
	temp_buff[9]=0xC2;
	temp_buff[10]=0x00;
	temp_buff[11]=0x3C;
	
	/*     CLIENT_ID      */
	temp_buff[12] = CLIENT_ID_LEN/256;                 //CLIENT_ID长度高字节
	temp_buff[13] = CLIENT_ID_LEN%256;                 //CLIENT_ID长度低字节
	memcpy(&temp_buff[14],CLIENT_ID,CLIENT_ID_LEN);   //复制过来CLIENT_ID字串
	
	/*     用户名        */
	temp_buff[14+CLIENT_ID_LEN] = USERNAME_ID_LEN/256; 				     //用户名长度高字节
	temp_buff[15+CLIENT_ID_LEN] = USERNAME_ID_LEN%256; 				     //用户名长度高字节
	memcpy(&temp_buff[16+CLIENT_ID_LEN],USERNAME_ID,USERNAME_ID_LEN);    //复制过来用户名字串
	
	/*      密码        */
	temp_buff[16+CLIENT_ID_LEN+USERNAME_ID_LEN] = PASSWARD_LEN/256;					  //密码长度高字节
	temp_buff[17+CLIENT_ID_LEN+USERNAME_ID_LEN] = PASSWARD_LEN%256;					  //密码长度高字节
	memcpy(&temp_buff[18+CLIENT_ID_LEN+USERNAME_ID_LEN],PASSWARD,PASSWARD_LEN);       //复制过来密码字串
 	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入数据到缓冲区
	
	return 0;	
}

/*----------------------------------------------------------*/
/*函数名：连接服务器报文                                    */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_CONNACK(u8 *data)
{
	u8 temp;
	
	switch(data[4])
	{
		case 0x00 : EMQTT_PRINTF("连接MQTT服务器成功\r\n");              temp = 0;break;
		case 0x01 : EMQTT_PRINTF("连接已拒绝，不支持的协议版本\r\n");    temp = 1;break;
		case 0x02 : EMQTT_PRINTF("连接已拒绝，不合格的客户端标识符\r\n");temp = 2;break;
		case 0x03 : EMQTT_PRINTF("连接已拒绝，服务端不可用\r\n");        temp = 3;break;
		case 0x04 : EMQTT_PRINTF("连接已拒绝，无效的用户名或密码\r\n");  temp = 4;break;
		case 0x05 : EMQTT_PRINTF("连接已拒绝，未授权\r\n");              temp = 5;break;		
	}
	
	return temp;
}

/*----------------------------------------------------------*/
/*函数名：SUBSCRIBE订阅topic报文                            */
/*参  数：QoS：订阅等级                                     */
/*参  数：topic_name：订阅topic报文名称                     */
/*返回值：0：正确  	其他：错误                              */
/*----------------------------------------------------------*/
u8 EMQTT_SUBSCRIBE(char *topic_name, u8 QoS)
{
    u16 Fixed_len;              //固定报头长度
	u16 Variable_len;           //可变报头长度
	u16 Payload_len;            //有效负荷长度
    u8  temp_buff[EMQTT_UNIT];	//临时缓冲区
	MessageOBD *temPtr;         //临时MessageOBD的指针
	
	if(QoS!=0&&QoS!=1&&QoS!=2)
	{
		EMQTT_PRINTF("SUBSCRIBE报文订阅服务等级错误\r\n");
		return 1;
	}
	
	Fixed_len = 2;                 //SUBSCRIBE报文中，固定报头长度=2
	Variable_len = 2;              //SUBSCRIBE报文中，可变报头长度=2
	
	/*------2字节(topic_name长度)+ topic_name字符串的长度 + 1字节服务等级*/
	Payload_len = 2 + strlen(topic_name) + 1;   //计算有效负荷长度
	
	temp_buff[0]=0x82;                            
	temp_buff[1]=Variable_len + Payload_len;	
	temp_buff[2]=Message_ID/256;
	temp_buff[3]=Message_ID%256;		
	temp_buff[4]=strlen(topic_name)/256;
	temp_buff[5]=strlen(topic_name)%256;		
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));	
	temp_buff[6+strlen(topic_name)]=QoS;
	
	if(EMQTT_MessageOBDPtr==NULL)  
		return 2;
	
	memcpy(EMQTT_MessageOBDPtr->MessageOBDBuf, temp_buff, Fixed_len + Variable_len + Payload_len);
	EMQTT_MessageOBDPtr->iD = Message_ID;
	EMQTT_MessageOBDPtr->data_len = Fixed_len + Variable_len + Payload_len;
	
	Message_ID++;
	if(Message_ID==0xFFFF)Message_ID=1;
	
	temPtr = EMQTT_MessageOBDPtr;
	EMQTT_MessageOBDPtr = EMQTT_MessageOBDPtr->NextPtr;
	
	if(First_MessageOBDPtr==NULL)
	{
		First_MessageOBDPtr = temPtr;
		First_MessageOBDPtr->NextPtr = NULL;
	}
	else
	{
		First_MessageOBDPtr->NextPtr = temPtr;
		temPtr->NextPtr =  NULL;
	}

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入数据到缓冲区

	return 0;
}

/*----------------------------------------------------------*/
/*函数名：SUBACK报文，订阅回复                              */
/*参  数：data：数据                                        */
/*返回值：0：正确 其他 错误                                 */
/*----------------------------------------------------------*/
u8 EMQTT_SUBACK(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD *temPtr;              //临时MessageOBD的指针
	MessageOBD  *temPrevPtr;         //临时MessageOBD的指针
	
	switch(data[5])
	{
		case 0x00 : EMQTT_PRINTF("订阅topic成功，等级0\r\n");temp = 0;break;
		case 0x01 : EMQTT_PRINTF("订阅topic成功，等级1\r\n");temp = 0;break;
		case 0x02 : EMQTT_PRINTF("订阅topic成功，等级2\r\n");temp = 0;break;
		case 0x80 : temp = 1;break;	
	}
	
    if(temp)	return 1;
	
	temp_id =  data[3]*256;  //计算报文标识符
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 2;
	
	temPtr = First_MessageOBDPtr;      //指向链表头;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //判断报文标识符是否相等
		{
			if(temPtr == First_MessageOBDPtr)
			{
				First_MessageOBDPtr = First_MessageOBDPtr->NextPtr;
			}
			else
			{
				temPrevPtr->NextPtr = temPtr->NextPtr;				
			}
			
			temPtr->NextPtr = EMQTT_MessageOBDPtr;
			EMQTT_MessageOBDPtr = temPtr;	
			temp = 0;
			temPtr = NULL;
		}
		else
		{
			temPrevPtr = temPtr;
			temPtr = temPtr->NextPtr;  //下一个
			temp = 3;
		}
	}
	
	return temp;	
}

/*----------------------------------------------------------*/
/*函数名：UNSUBSCRIBE取消订阅topic报文                      */
/*参  数：topic_name：topic报文名称                         */
/*返回值：0：正确  	其他：错误                              */
/*----------------------------------------------------------*/
u8 EMQTT_UNSUBSCRIBE(char *topic_name)
{
    u16 Fixed_len;              //固定报头长度
	u16 Variable_len;           //可变报头长度
	u16 Payload_len;            //有效负荷长度
    u8  temp_buff[EMQTT_UNIT];	//临时缓冲区
	MessageOBD *temPtr;         //临时MessageOBD的指针
	
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic错误，不能为空\r\n");
		return 1;
	}
	
	Fixed_len = 2;                 //UNSUBSCRIBE报文中，固定报头长度=2
	Variable_len = 2;              //UNSUBSCRIBE报文中，可变报头长度=2
	
	/*------2字节(topic_name长度)+ topic_name字符串的长度*/
	Payload_len = 2 + strlen(topic_name);   //计算有效负荷长度
	
	temp_buff[0]=0xA2;                            
	temp_buff[1]=Variable_len + Payload_len;	
	temp_buff[2]=Message_ID/256;
	temp_buff[3]=Message_ID%256;		
	temp_buff[4]=strlen(topic_name)/256;
	temp_buff[5]=strlen(topic_name)%256;		
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));	
	
	if(EMQTT_MessageOBDPtr==NULL)  
		return 2;
	
	memcpy(EMQTT_MessageOBDPtr->MessageOBDBuf, temp_buff, Fixed_len + Variable_len + Payload_len);
	EMQTT_MessageOBDPtr->iD = Message_ID;
	EMQTT_MessageOBDPtr->data_len = Fixed_len + Variable_len + Payload_len;
	
	Message_ID++;
	if(Message_ID==0xFFFF)Message_ID=1;
	
	temPtr = EMQTT_MessageOBDPtr;
	EMQTT_MessageOBDPtr = EMQTT_MessageOBDPtr->NextPtr;
	
	if(First_MessageOBDPtr==NULL)
	{
		First_MessageOBDPtr = temPtr;
		First_MessageOBDPtr->NextPtr = NULL;
	}
	else
	{
		First_MessageOBDPtr->NextPtr = temPtr;
		temPtr->NextPtr =  NULL;
	}

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入数据到缓冲区

	return 0;
}

/*----------------------------------------------------------*/
/*函数名：UNSUBACK报文，取消订阅回复                        */
/*参  数：data：数据                                        */
/*返回值：0：正确 其他 错误                                 */
/*----------------------------------------------------------*/
u8 EMQTT_UNSUBACK(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD  *temPtr;             //临时MessageOBD的指针
	MessageOBD  *temPrevPtr;         //临时MessageOBD的指针
	
	temp_id =  data[3]*256;          //计算报文标识符
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 1;
	
	temPtr = First_MessageOBDPtr;      //指向链表头;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //判断报文标识符是否相等
		{
			if(temPtr == First_MessageOBDPtr)
			{
				First_MessageOBDPtr = First_MessageOBDPtr->NextPtr;
			}
			else
			{
				temPrevPtr->NextPtr = temPtr->NextPtr;				
			}
			
			temPtr->NextPtr = EMQTT_MessageOBDPtr;
			EMQTT_MessageOBDPtr = temPtr;	
			temp = 0;
			temPtr = NULL;
		}
		else
		{
			temPrevPtr = temPtr;
			temPtr = temPtr->NextPtr;  //下一个
			temp = 2;
		}
	}
	
	return temp;	
}

/*----------------------------------------------------------*/
/*函数名：等级0 发布消息报文                                */
/*参  数：topic_name：topic名称                             */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISH_Qs0(char *topic_name, u8 *data)
{
    u16 Fixed_len;              //固定报头长度
	u16 Variable_len;           //可变报头长度
	u16 Payload_len;            //有效负荷长度
    u8  temp_buff[EMQTT_NUM];	//临时缓冲区
//	u8  temp_buff[300];	//临时缓冲区
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic名称错误，不能为空\r\n");
		return 1;
	}
	
	if(data == NULL)
	{
		EMQTT_PRINTF("数据错误，不能为空\r\n");
		return 2;
	}
	
	Fixed_len = 2;                           //固定报头长度=2
	
	/*------2字节(topic_name长度)+ topic_name字符串的长度*/
	Variable_len = 2 + strlen(topic_name);   //计算可变报头长度
	
	/*------data数据字符串的长度*/
	Payload_len = strlen((char *)data);      //计算有效负荷长度
	
	temp_buff[0]=0x30;                            
	temp_buff[1]=Variable_len + Payload_len;		
	temp_buff[2]=strlen(topic_name)/256;
	temp_buff[3]=strlen(topic_name)%256;		
	memcpy(&temp_buff[4],topic_name,strlen(topic_name));	
	memcpy(&temp_buff[4+strlen(topic_name)],data,strlen((char *)data));
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入数据到缓冲区
	
	return 0;
}

/*----------------------------------------------------------*/
/*函数名：处理收到的消息报文 等级0                          */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_Deal_Qs0(u8 *data)
{
	u8 data_len;                //数据总长度
	u16 topic_len;              //topic名称长度
	u8 temp_topic[EMQTT_UNIT];  //存放topic名称的临时缓冲区
	u8 temp_buff[EMQTT_UNIT];	//临时缓冲区
	
	data_len = data[0];                     //获取数据总长度
	
	topic_len  = data[3]*256;               //获取topic名称长度
	topic_len += data[4];	
	memcpy(temp_topic,&data[5],topic_len);                              //topic名称拷贝出来
	memcpy(temp_buff,&data[5+topic_len],data_len - 2 - 2 - topic_len);  //数据拷贝出来
		
	if(!memcmp(temp_topic,Cntr_CMD,strlen(Cntr_CMD))) //如果topic是Cntr_CMD，说明是控制端发送的数据
	{                     
		CMDBuf_Deal(temp_buff, data_len - 2 - 2 - topic_len);  //加入命令到缓冲区
	}
	else  //如果topic不是Cntr_CMD，说明不是控制端发送的数据，数据丢弃
	{
		return 1;     
	}
	
	return 0;
}

/*----------------------------------------------------------*/
/*函数名：等级1 发布消息报文                                */
/*参  数：topic_name：topic名称                             */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISH_Qs1(char *topic_name, u8 *data)
{
    u16 Fixed_len;              //固定报头长度
	u16 Variable_len;           //可变报头长度
	u16 Payload_len;            //有效负荷长度
    u8  temp_buff[EMQTT_UNIT];	//临时缓冲区
	MessageOBD *temPtr;         //临时MessageOBD的指针
	
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic名称错误，不能为空\r\n");
		return 1;
	}
	
	if(data == NULL)
	{
		EMQTT_PRINTF("数据错误，不能为空\r\n");
		return 2;
	}
	
	Fixed_len = 2;                               //固定报头长度=2
	
	/*------2字节(topic_name长度)+ topic_name字符串的长度 + 2字节报文表示符*/
	Variable_len = 2 + strlen(topic_name) + 2;   //计算可变报头长度
	
	/*------data数据字符串的长度*/
	Payload_len = strlen((char *)data);          //计算有效负荷长度
	
	temp_buff[0]=0x32;                            
	temp_buff[1]=Variable_len + Payload_len;		
	temp_buff[2]=strlen(topic_name)/256;
	temp_buff[3]=strlen(topic_name)%256;		
	memcpy(&temp_buff[4],topic_name,strlen(topic_name));	
	temp_buff[4+strlen(topic_name)]=Message_ID/256;
	temp_buff[5+strlen(topic_name)]=Message_ID%256;		
	memcpy(&temp_buff[6+strlen(topic_name)],data,strlen((char *)data));
	
	if(EMQTT_MessageOBDPtr==NULL)  
		return 2;
	
	memcpy(EMQTT_MessageOBDPtr->MessageOBDBuf, temp_buff, Fixed_len + Variable_len + Payload_len);
	EMQTT_MessageOBDPtr->iD = Message_ID;
	EMQTT_MessageOBDPtr->data_len = Fixed_len + Variable_len + Payload_len;
	
	Message_ID++;
	if(Message_ID==0xFFFF)Message_ID=1;
	
	temPtr = EMQTT_MessageOBDPtr;
	EMQTT_MessageOBDPtr = EMQTT_MessageOBDPtr->NextPtr;
	
	if(First_MessageOBDPtr==NULL)
	{
		First_MessageOBDPtr = temPtr;
		First_MessageOBDPtr->NextPtr = NULL;
	}
	else
	{
		First_MessageOBDPtr->NextPtr = temPtr;
		temPtr->NextPtr =  NULL;
	}

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入数据到缓冲区

	return 0;
}

/*----------------------------------------------------------*/
/*函数名：处理收到的消息报文 等级1                          */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_Deal_Qs1(u8 *data)
{
	u8 data_len;                //数据总长度
	u16 topic_len;              //topic名称长度
	u8 temp_topic[EMQTT_UNIT];  //存放topic名称的临时缓冲区
	u8 temp_buff[EMQTT_UNIT];	//临时缓冲区	
	u8 temp[4];                 //临时缓冲区
	
	data_len = data[0];                     //获取数据总长度
	
	topic_len  = data[3]*256;               //获取topic名称长度
	topic_len += data[4];	
	
	memcpy(temp_topic,&data[5],topic_len);                                     //topic名称拷贝出来
	memcpy(temp_buff,&data[5+topic_len +2],data_len - 2 - 2 - 2 - topic_len);  //数据拷贝出来
	
	temp[0]=0x40; 
	temp[1]=0x02;
	temp[2]=data[5+topic_len];
	temp[3]=data[6+topic_len];
	TxDataBuf_Deal(temp, 4);    //加入数据到缓冲区,等级1的推送，我们要恢复服务器，表示接收到该推送

	if(!memcmp(temp_topic,Cntr_CMD,strlen(Cntr_CMD))) //如果topic是Cntr_CMD，说明是控制端发送的数据
	{                     
		CMDBuf_Deal(temp_buff, data_len - 2 - 2 - 2 - topic_len);   //加入命令到缓冲区
	}
	else  //如果topic不是Cntr_CMD，说明不是控制端发送的数据，数据丢弃
	{
		return 1;     
	}
	
	return 0;
}

/*----------------------------------------------------------*/
/*函数名：处理等级1推送报文回复                             */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISHQs1_ACK(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD  *temPtr;             //临时MessageOBD的指针
	MessageOBD  *temPrevPtr;         //临时MessageOBD的指针
	
	temp_id =  data[3]*256;          //计算报文标识符
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 1;
	
	temPtr = First_MessageOBDPtr;      //指向链表头;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //判断报文标识符是否相等
		{
			if(temPtr == First_MessageOBDPtr)
			{
				First_MessageOBDPtr = First_MessageOBDPtr->NextPtr;
			}
			else
			{
				temPrevPtr->NextPtr = temPtr->NextPtr;				
			}
			
			temPtr->NextPtr = EMQTT_MessageOBDPtr;
			EMQTT_MessageOBDPtr = temPtr;	
			temp = 0;
			temPtr = NULL;
		}
		else
		{
			temPrevPtr = temPtr;
			temPtr = temPtr->NextPtr;  //下一个
			temp = 2;
		}
	}
	
	return temp;
}

/*----------------------------------------------------------*/
/*函数名：等级2 发布消息报文                                */
/*参  数：topic_name：topic名称                             */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISH_Qs2(char *topic_name, u8 *data)
{
    u16 Fixed_len;              //固定报头长度
	u16 Variable_len;           //可变报头长度
	u16 Payload_len;            //有效负荷长度
    u8  temp_buff[EMQTT_UNIT];	//临时缓冲区
	MessageOBD *temPtr;         //临时MessageOBD的指针
	
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic名称错误，不能为空\r\n");
		return 1;
	}
	
	if(data == NULL)
	{
		EMQTT_PRINTF("数据错误，不能为空\r\n");
		return 2;
	}
	
	Fixed_len = 2;                               //固定报头长度=2
	
	/*------2字节(topic_name长度)+ topic_name字符串的长度 + 2字节报文表示符*/
	Variable_len = 2 + strlen(topic_name) + 2;   //计算可变报头长度
	
	/*------data数据字符串的长度*/
	Payload_len = strlen((char *)data);          //计算有效负荷长度
	
	temp_buff[0]=0x34;                            
	temp_buff[1]=Variable_len + Payload_len;		
	temp_buff[2]=strlen(topic_name)/256;
	temp_buff[3]=strlen(topic_name)%256;		
	memcpy(&temp_buff[4],topic_name,strlen(topic_name));	
	temp_buff[4+strlen(topic_name)]=Message_ID/256;
	temp_buff[5+strlen(topic_name)]=Message_ID%256;		
	memcpy(&temp_buff[6+strlen(topic_name)],data,strlen((char *)data));
	
	if(EMQTT_MessageOBDPtr==NULL)  
		return 2;
	
	memcpy(EMQTT_MessageOBDPtr->MessageOBDBuf, temp_buff, Fixed_len + Variable_len + Payload_len);
	EMQTT_MessageOBDPtr->iD = Message_ID;
	EMQTT_MessageOBDPtr->data_len = Fixed_len + Variable_len + Payload_len;
	
	Message_ID++;
	if(Message_ID==0xFFFF)Message_ID=1;
	
	temPtr = EMQTT_MessageOBDPtr;
	EMQTT_MessageOBDPtr = EMQTT_MessageOBDPtr->NextPtr;
	
	if(First_MessageOBDPtr==NULL)
	{
		First_MessageOBDPtr = temPtr;
		First_MessageOBDPtr->NextPtr = NULL;
	}
	else
	{
		First_MessageOBDPtr->NextPtr = temPtr;
		temPtr->NextPtr =  NULL;
	}

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //加入数据到缓冲区

	return 0;
}

/*----------------------------------------------------------*/
/*函数名：等级2发布收到                                     */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void EMQTT_PUBREC(u8 *data)
{
	u16 temp_id;                //存放报文标识符
		
	temp_id =  data[3]*256;     //计算报文标识符
	temp_id += data[4];
	
    EMQTT_PUBREL(temp_id);      //发送等级2发布释放报文	
}

/*----------------------------------------------------------*/
/*函数名：等级2发布释放                                     */
/*参  数：iD：报文表示符                                    */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void EMQTT_PUBREL(u16 iD)
{
	u8 temp[4];                 //临时缓冲区
		
	temp[0]=0x62; 
	temp[1]=0x02;
	temp[2]=iD/256;
	temp[3]=iD%256;
	TxDataBuf_Deal(temp, 4);    //加入数据到缓冲区,等级1的推送，我们要恢复服务器，表示接收到该推送
}

/*----------------------------------------------------------*/
/*函数名：等级2发布完成报文                                 */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBCOMP(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD  *temPtr;             //临时MessageOBD的指针
	MessageOBD  *temPrevPtr;         //临时MessageOBD的指针
	
	temp_id =  data[3]*256;          //计算报文标识符
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 1;
	
	temPtr = First_MessageOBDPtr;      //指向链表头;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //判断报文标识符是否相等
		{
			if(temPtr == First_MessageOBDPtr)
			{
				First_MessageOBDPtr = First_MessageOBDPtr->NextPtr;
			}
			else
			{
				temPrevPtr->NextPtr = temPtr->NextPtr;				
			}
			
			temPtr->NextPtr = EMQTT_MessageOBDPtr;
			EMQTT_MessageOBDPtr = temPtr;	
			temp = 0;
			temPtr = NULL;
		}
		else
		{
			temPrevPtr = temPtr;
			temPtr = temPtr->NextPtr;  //下一个
			temp = 2;
		}
	}
	
	return temp;
}

/*----------------------------------------------------------*/
/*函数名：处理收到的消息报文 等级2                          */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_Deal_Qs2(u8 *data)
{
	u8 data_len;                //数据总长度
	u16 topic_len;              //topic名称长度
	u8 temp_topic[EMQTT_UNIT];  //存放topic名称的临时缓冲区
	u8 temp_buff[EMQTT_UNIT];	//临时缓冲区	
	u8 temp[4];                 //临时缓冲区
	
	data_len = data[0];                     //获取数据总长度
	
	topic_len  = data[3]*256;               //获取topic名称长度
	topic_len += data[4];	
	
	memcpy(temp_topic,&data[5],topic_len);                                     //topic名称拷贝出来
	memcpy(temp_buff,&data[5+topic_len +2],data_len - 2 - 2 - 2 - topic_len);  //数据拷贝出来
	
	temp[0]=0x50; 
	temp[1]=0x02;
	temp[2]=data[5+topic_len];
	temp[3]=data[6+topic_len];
	TxDataBuf_Deal(temp, 4);    //加入数据到缓冲区,等级2的推送，我们要回复服务器，表示接收到该推送

	if(!memcmp(temp_topic,Cntr_CMD,strlen(Cntr_CMD))) //如果topic是Cntr_CMD，说明是控制端发送的数据
	{                     
		CMDBuf_Deal(temp_buff, data_len - 2 - 2 - 2 - topic_len);   //加入命令到缓冲区
	}
	else  //如果topic不是Cntr_CMD，说明不是控制端发送的数据，数据丢弃
	{
		return 1;     
	}
	
	return 0;
}

/*----------------------------------------------------------*/
/*函数名：等级2回复发布完成                                 */
/*参  数：data：数据                                        */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void EMQTT_DealQs2_ACK(u8 *data)
{
	u8 temp[4];                //缓冲区
		
	temp[0]=0x70; 
	temp[1]=0x02;
	temp[2]=data[3];
	temp[3]=data[4];
	
	TxDataBuf_Deal(temp, 4);    //加入数据到缓冲区,等级1的推送，我们要恢复服务器，表示接收到该推送
}

/*----------------------------------------------------------*/
/*函数名：PING服务器报文，保活用                            */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void EMQTT_PINGREQ(void)
{
	u8  temp_buff[2]={0xC0, 0x00};	//PING报文内容

	TxDataBuf_Deal(temp_buff, 2);   //加入数据到缓冲区
}

/*----------------------------------------------------------*/
/*函数名：PING响应报文，服务器的回复                        */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PINGRESP(u8 *data)
{
	if(data[2]!=0x00) return 1;
 
    return 0;	
}

/*----------------------------------------------------------*/
/*函数名：断开连接报文                                      */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void EMQTT_DISCONNECT(void)
{
	u8  temp_buff[2]={0xE0, 0x00};	//断开连接报文内容

	TxDataBuf_Deal(temp_buff, 2);   //加入数据到缓冲区
}

/*----------------------------------------------------------*/
/*函数名：处理发送缓冲区                                    */
/*参  数：data：数据                                        */
/*参  数：size：数据长度                                    */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(u8 *data, u8 size)
{
	memcpy(&EMQTT_TxDataInPtr[1],data,size);      //拷贝数据到发送缓冲区
	EMQTT_TxDataInPtr[0] = size;                  //记录数据长度
	EMQTT_TxDataInPtr+=EMQTT_UNIT;                //指针下移
	if(EMQTT_TxDataInPtr==EMQTT_TxDataEndPtr)     //如果指针到缓冲区尾部了
		EMQTT_TxDataInPtr = EMQTT_TxDataBuf[0];   //指针归位到缓冲区开头
}

/*----------------------------------------------------------*/
/*函数名：发送数据                                          */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void EMQTT_TxData(void)
{
	char temp_2[50]={0};
	
	SIM800C_TxData(EMQTT_TxDataOutPtr);
	neul_bc95_str_to_hex((const unsigned char *)EMQTT_TxDataOutPtr,strlen((const char *)EMQTT_TxDataOutPtr),temp_2);
	UartWriteBuffer_Config((u8 *)temp_2,strlen((char *)temp_2));	
}

/*----------------------------------------------------------*/
/*函数名：处理命令缓冲区                                    */
/*参  数：data：数据                                        */
/*参  数：size：数据长度                                    */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(u8 *data, u8 size)
{
	memcpy(&EMQTT_CMDInPtr[1],data,size);      //拷贝数据到命令缓冲区
	EMQTT_CMDInPtr[0] = size;                  //记录数据长度
	EMQTT_CMDInPtr[size+1] = '\0';             //加入结束符
	EMQTT_CMDInPtr+=EMQTT_UNIT;                //指针下移
	if(EMQTT_CMDInPtr==EMQTT_CMDEndPtr)        //如果指针到缓冲区尾部了
		EMQTT_CMDInPtr = EMQTT_CMDBuf[0];      //指针归位到缓冲区开头
}

/* ============================================================
func name   :  neul_bc95_hex_to_str
discription :  convert hex string to hex data 
               
param       :  bufin @ input param , the data that need to convert
               len @ input param, data length
               bufout @ output param, hex data after convert 
return      :  0 send ok , !0 not ok
Revision    : 
=============================================================== */
int neul_bc95_str_to_hex(const unsigned char *bufin, int len, char *bufout)
{
    int i = 0;
    #if 1
    int tmp = 0;
    #endif
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for(i = 0; i < len; i++)
    {
        #if 1
        tmp = bufin[i]>>4;
        bufout[i*2] = tmp > 0x09?tmp+0x37:tmp+0x30;
        tmp = bufin[i]&0x0F;
        bufout[i*2+1] = tmp > 0x09?tmp+0x37:tmp+0x30;
        #else
        sprintf(bufout+i*2, "%02X", bufin[i]);
        #endif
    }
    return 0; 
}

/* ============================================================
func name   :  neul_bc95_hex_to_str
discription :  convert char to hex string 
               
param       :  bufin @ input param , the data that need to convert
               len @ input param, data length
               bufout @ output param, hex string data 
return      :  0 convert ok , !0 not ok
Revision    : 
=============================================================== */
int neul_bc95_hex_to_str(const char *bufin, int len, char *bufout)
{
    int i = 0;
    unsigned char tmp2 = 0x0;
    unsigned int tmp = 0;
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for(i = 0; i < len; i = i+2)
    {
        #if 1
        tmp2 =  bufin[i];
        tmp2 =  tmp2 <= '9'?tmp2-0x30:tmp2-0x37;
        tmp =  bufin[i+1];
        tmp =  tmp <= '9'?tmp-0x30:tmp-0x37;
        bufout[i/2] =(tmp2<<4)|(tmp&0x0F);
        #else
        sscanf(bufin+i, "%02x", &tmp);
        bufout[i/2] = tmp;
        #endif
    }
    return 0; 
}

/*------------- 从字符串的中间截取n个字符 --------------*/
//char * mid(char *dst,char *src, int nLen,int nPos)
//{
//    char *p = src;
//    char *q = dst;
//    int len = strlen(src);
//    if(nLen>len) nLen = len-nPos;    /*从第nPos个到最后*/
//    if(nPos<0) nPos=0;    /*从第一个开始*/
//    if(nPos>len) return NULL;
//    p += nPos;
//    while(nLen--) *(q++) = *(p++);
//    *(q++)='\0'; /*有必要吗?很有必要*/
//    return dst;
//}
