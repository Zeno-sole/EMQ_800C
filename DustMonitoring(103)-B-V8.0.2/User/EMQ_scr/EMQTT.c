/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "EMQTT.h" 
#include "sim800c.h"
#include "sim800c_usart.h"
//#include "usart1.h"
//#include "led.h"

//#define  CLIENT_ID           "518727330"                       //�ͻ���ID (  �豸ID  )  //ʹ��UID����STM32Ψһ��ʶ��
char *CLIENT_ID = "518727330";
#define  CLIENT_ID_LEN       strlen(CLIENT_ID)                //����

#define  USERNAME_ID         "217577"                         //�û���   (  ��ƷID  )
#define  USERNAME_ID_LEN     strlen(USERNAME_ID)              //����

#define  PASSWARD            "kfbskd"                         //����     ( ��Ȩ��Ϣ )
#define  PASSWARD_LEN         strlen(PASSWARD)                //����

u8  EMQTT_RxDataBuf[EMQTT_NUM][EMQTT_UNIT];       //���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
u8 *EMQTT_RxDataInPtr;                            //ָ�򻺳���������ݵ�λ��
u8 *EMQTT_RxDataOutPtr;                           //ָ�򻺳�����ȡ���ݵ�λ��
u8 *EMQTT_RxDataEndPtr;                           //ָ�򻺳���������λ��

u8  EMQTT_TxDataBuf[EMQTT_NUM][EMQTT_UNIT];       //���ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
u8 *EMQTT_TxDataInPtr;                            //ָ�򻺳���������ݵ�λ��
u8 *EMQTT_TxDataOutPtr;                           //ָ�򻺳�����ȡ���ݵ�λ��
u8 *EMQTT_TxDataEndPtr;                           //ָ�򻺳���������λ��

u8  EMQTT_CMDBuf[CMD_NUM][EMQTT_UNIT];            //�������ݵĽ��ջ�����
u8 *EMQTT_CMDInPtr;                               //ָ�򻺳���������ݵ�λ��
u8 *EMQTT_CMDOutPtr;                              //ָ�򻺳�����ȡ���ݵ�λ��
u8 *EMQTT_CMDEndPtr;                              //ָ�򻺳���������λ��

u16            Message_ID=1;                      //���ı�ʶ����ÿ�����Ķ�Ӧ�÷���һ����ͬ��δռ�õı�ʶ��
MessageOBD     EMQTT_MessageOBD[MessageOBD_NUM];  //�б��ı�ʶ���ı��ĵĹ���ṹ��
MessageOBD    *EMQTT_MessageOBDPtr;               //ָ��MessageOBD�ṹ������ͷ��ָ��
MessageOBD    *First_MessageOBDPtr;               //��ռ�õ�MessageOBD�ṹ������ͷ��ָ��



/*----------------------------------------------------------*/
/*��������EMQTT��ʼ��                                       */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void EMQTT_Init(void)
{	
	u8 i;
	
	Message_ID = 1;                                  //���ı�ʶ��,�ص�1
	
	EMQTT_RxDataInPtr=EMQTT_RxDataBuf[0];            //ָ���λ
	EMQTT_RxDataOutPtr=EMQTT_RxDataInPtr;            //ָ���λ
    EMQTT_RxDataEndPtr=EMQTT_RxDataBuf[EMQTT_NUM-1]; //ָ���λ
	
	EMQTT_TxDataInPtr=EMQTT_TxDataBuf[0];            //ָ���λ
	EMQTT_TxDataOutPtr=EMQTT_TxDataInPtr;            //ָ���λ
	EMQTT_TxDataEndPtr=EMQTT_TxDataBuf[EMQTT_NUM-1]; //ָ���λ
	
	EMQTT_CMDInPtr=EMQTT_CMDBuf[0];                  //ָ���λ
	EMQTT_CMDOutPtr=EMQTT_CMDInPtr;                  //ָ���λ
	EMQTT_CMDEndPtr=EMQTT_CMDBuf[CMD_NUM-1];         //ָ���λ	
	
	for(i=0;i<(MessageOBD_NUM-1);i++)                //��������
	{ 
		EMQTT_MessageOBD[i].NextPtr = &EMQTT_MessageOBD[i+1];
	}
	EMQTT_MessageOBD[MessageOBD_NUM-1].NextPtr = NULL;
	EMQTT_MessageOBDPtr = &EMQTT_MessageOBD[0];
	First_MessageOBDPtr = NULL;
}

/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ����will_sta��0 �������������  1 �����������        */
/*��  ����will_topic������topic����                         */
/*��  ����will_content������topic����                       */
/*��  ����will_QoS������topic����ȼ�                       */
/*����ֵ��0���ɹ�  ������ ʧ��                              */
/*----------------------------------------------------------*/
u8 EMQTT_CONNECT(void)
{
    u16 Fixed_len;              //�̶���ͷ����
	u16 Variable_len;           //�ɱ䱨ͷ����
	u16 Payload_len;            //��Ч���ɳ���
    u8  temp_buff[EMQTT_UNIT];	//��ʱ������

	Fixed_len = 2;                 //���ӱ����У��̶���ͷ����=2
	Variable_len = 10;             //���ӱ����У��ɱ䱨ͷ����=10
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
	temp_buff[12] = CLIENT_ID_LEN/256;                 //CLIENT_ID���ȸ��ֽ�
	temp_buff[13] = CLIENT_ID_LEN%256;                 //CLIENT_ID���ȵ��ֽ�
	memcpy(&temp_buff[14],CLIENT_ID,CLIENT_ID_LEN);   //���ƹ���CLIENT_ID�ִ�
	
	/*     �û���        */
	temp_buff[14+CLIENT_ID_LEN] = USERNAME_ID_LEN/256; 				     //�û������ȸ��ֽ�
	temp_buff[15+CLIENT_ID_LEN] = USERNAME_ID_LEN%256; 				     //�û������ȸ��ֽ�
	memcpy(&temp_buff[16+CLIENT_ID_LEN],USERNAME_ID,USERNAME_ID_LEN);    //���ƹ����û����ִ�
	
	/*      ����        */
	temp_buff[16+CLIENT_ID_LEN+USERNAME_ID_LEN] = PASSWARD_LEN/256;					  //���볤�ȸ��ֽ�
	temp_buff[17+CLIENT_ID_LEN+USERNAME_ID_LEN] = PASSWARD_LEN%256;					  //���볤�ȸ��ֽ�
	memcpy(&temp_buff[18+CLIENT_ID_LEN+USERNAME_ID_LEN],PASSWARD,PASSWARD_LEN);       //���ƹ��������ִ�
 	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //�������ݵ�������
	
	return 0;	
}

/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_CONNACK(u8 *data)
{
	u8 temp;
	
	switch(data[4])
	{
		case 0x00 : EMQTT_PRINTF("����MQTT�������ɹ�\r\n");              temp = 0;break;
		case 0x01 : EMQTT_PRINTF("�����Ѿܾ�����֧�ֵ�Э��汾\r\n");    temp = 1;break;
		case 0x02 : EMQTT_PRINTF("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ��\r\n");temp = 2;break;
		case 0x03 : EMQTT_PRINTF("�����Ѿܾ�������˲�����\r\n");        temp = 3;break;
		case 0x04 : EMQTT_PRINTF("�����Ѿܾ�����Ч���û���������\r\n");  temp = 4;break;
		case 0x05 : EMQTT_PRINTF("�����Ѿܾ���δ��Ȩ\r\n");              temp = 5;break;		
	}
	
	return temp;
}

/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic����                            */
/*��  ����QoS�����ĵȼ�                                     */
/*��  ����topic_name������topic��������                     */
/*����ֵ��0����ȷ  	����������                              */
/*----------------------------------------------------------*/
u8 EMQTT_SUBSCRIBE(char *topic_name, u8 QoS)
{
    u16 Fixed_len;              //�̶���ͷ����
	u16 Variable_len;           //�ɱ䱨ͷ����
	u16 Payload_len;            //��Ч���ɳ���
    u8  temp_buff[EMQTT_UNIT];	//��ʱ������
	MessageOBD *temPtr;         //��ʱMessageOBD��ָ��
	
	if(QoS!=0&&QoS!=1&&QoS!=2)
	{
		EMQTT_PRINTF("SUBSCRIBE���Ķ��ķ���ȼ�����\r\n");
		return 1;
	}
	
	Fixed_len = 2;                 //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;              //SUBSCRIBE�����У��ɱ䱨ͷ����=2
	
	/*------2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�*/
	Payload_len = 2 + strlen(topic_name) + 1;   //������Ч���ɳ���
	
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

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //�������ݵ�������

	return 0;
}

/*----------------------------------------------------------*/
/*��������SUBACK���ģ����Ļظ�                              */
/*��  ����data������                                        */
/*����ֵ��0����ȷ ���� ����                                 */
/*----------------------------------------------------------*/
u8 EMQTT_SUBACK(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD *temPtr;              //��ʱMessageOBD��ָ��
	MessageOBD  *temPrevPtr;         //��ʱMessageOBD��ָ��
	
	switch(data[5])
	{
		case 0x00 : EMQTT_PRINTF("����topic�ɹ����ȼ�0\r\n");temp = 0;break;
		case 0x01 : EMQTT_PRINTF("����topic�ɹ����ȼ�1\r\n");temp = 0;break;
		case 0x02 : EMQTT_PRINTF("����topic�ɹ����ȼ�2\r\n");temp = 0;break;
		case 0x80 : temp = 1;break;	
	}
	
    if(temp)	return 1;
	
	temp_id =  data[3]*256;  //���㱨�ı�ʶ��
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 2;
	
	temPtr = First_MessageOBDPtr;      //ָ������ͷ;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //�жϱ��ı�ʶ���Ƿ����
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
			temPtr = temPtr->NextPtr;  //��һ��
			temp = 3;
		}
	}
	
	return temp;	
}

/*----------------------------------------------------------*/
/*��������UNSUBSCRIBEȡ������topic����                      */
/*��  ����topic_name��topic��������                         */
/*����ֵ��0����ȷ  	����������                              */
/*----------------------------------------------------------*/
u8 EMQTT_UNSUBSCRIBE(char *topic_name)
{
    u16 Fixed_len;              //�̶���ͷ����
	u16 Variable_len;           //�ɱ䱨ͷ����
	u16 Payload_len;            //��Ч���ɳ���
    u8  temp_buff[EMQTT_UNIT];	//��ʱ������
	MessageOBD *temPtr;         //��ʱMessageOBD��ָ��
	
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic���󣬲���Ϊ��\r\n");
		return 1;
	}
	
	Fixed_len = 2;                 //UNSUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;              //UNSUBSCRIBE�����У��ɱ䱨ͷ����=2
	
	/*------2�ֽ�(topic_name����)+ topic_name�ַ����ĳ���*/
	Payload_len = 2 + strlen(topic_name);   //������Ч���ɳ���
	
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

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //�������ݵ�������

	return 0;
}

/*----------------------------------------------------------*/
/*��������UNSUBACK���ģ�ȡ�����Ļظ�                        */
/*��  ����data������                                        */
/*����ֵ��0����ȷ ���� ����                                 */
/*----------------------------------------------------------*/
u8 EMQTT_UNSUBACK(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD  *temPtr;             //��ʱMessageOBD��ָ��
	MessageOBD  *temPrevPtr;         //��ʱMessageOBD��ָ��
	
	temp_id =  data[3]*256;          //���㱨�ı�ʶ��
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 1;
	
	temPtr = First_MessageOBDPtr;      //ָ������ͷ;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //�жϱ��ı�ʶ���Ƿ����
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
			temPtr = temPtr->NextPtr;  //��һ��
			temp = 2;
		}
	}
	
	return temp;	
}

/*----------------------------------------------------------*/
/*���������ȼ�0 ������Ϣ����                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISH_Qs0(char *topic_name, u8 *data)
{
    u16 Fixed_len;              //�̶���ͷ����
	u16 Variable_len;           //�ɱ䱨ͷ����
	u16 Payload_len;            //��Ч���ɳ���
    u8  temp_buff[EMQTT_NUM];	//��ʱ������
//	u8  temp_buff[300];	//��ʱ������
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic���ƴ��󣬲���Ϊ��\r\n");
		return 1;
	}
	
	if(data == NULL)
	{
		EMQTT_PRINTF("���ݴ��󣬲���Ϊ��\r\n");
		return 2;
	}
	
	Fixed_len = 2;                           //�̶���ͷ����=2
	
	/*------2�ֽ�(topic_name����)+ topic_name�ַ����ĳ���*/
	Variable_len = 2 + strlen(topic_name);   //����ɱ䱨ͷ����
	
	/*------data�����ַ����ĳ���*/
	Payload_len = strlen((char *)data);      //������Ч���ɳ���
	
	temp_buff[0]=0x30;                            
	temp_buff[1]=Variable_len + Payload_len;		
	temp_buff[2]=strlen(topic_name)/256;
	temp_buff[3]=strlen(topic_name)%256;		
	memcpy(&temp_buff[4],topic_name,strlen(topic_name));	
	memcpy(&temp_buff[4+strlen(topic_name)],data,strlen((char *)data));
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //�������ݵ�������
	
	return 0;
}

/*----------------------------------------------------------*/
/*�������������յ�����Ϣ���� �ȼ�0                          */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_Deal_Qs0(u8 *data)
{
	u8 data_len;                //�����ܳ���
	u16 topic_len;              //topic���Ƴ���
	u8 temp_topic[EMQTT_UNIT];  //���topic���Ƶ���ʱ������
	u8 temp_buff[EMQTT_UNIT];	//��ʱ������
	
	data_len = data[0];                     //��ȡ�����ܳ���
	
	topic_len  = data[3]*256;               //��ȡtopic���Ƴ���
	topic_len += data[4];	
	memcpy(temp_topic,&data[5],topic_len);                              //topic���ƿ�������
	memcpy(temp_buff,&data[5+topic_len],data_len - 2 - 2 - topic_len);  //���ݿ�������
		
	if(!memcmp(temp_topic,Cntr_CMD,strlen(Cntr_CMD))) //���topic��Cntr_CMD��˵���ǿ��ƶ˷��͵�����
	{                     
		CMDBuf_Deal(temp_buff, data_len - 2 - 2 - topic_len);  //�������������
	}
	else  //���topic����Cntr_CMD��˵�����ǿ��ƶ˷��͵����ݣ����ݶ���
	{
		return 1;     
	}
	
	return 0;
}

/*----------------------------------------------------------*/
/*���������ȼ�1 ������Ϣ����                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISH_Qs1(char *topic_name, u8 *data)
{
    u16 Fixed_len;              //�̶���ͷ����
	u16 Variable_len;           //�ɱ䱨ͷ����
	u16 Payload_len;            //��Ч���ɳ���
    u8  temp_buff[EMQTT_UNIT];	//��ʱ������
	MessageOBD *temPtr;         //��ʱMessageOBD��ָ��
	
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic���ƴ��󣬲���Ϊ��\r\n");
		return 1;
	}
	
	if(data == NULL)
	{
		EMQTT_PRINTF("���ݴ��󣬲���Ϊ��\r\n");
		return 2;
	}
	
	Fixed_len = 2;                               //�̶���ͷ����=2
	
	/*------2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 2�ֽڱ��ı�ʾ��*/
	Variable_len = 2 + strlen(topic_name) + 2;   //����ɱ䱨ͷ����
	
	/*------data�����ַ����ĳ���*/
	Payload_len = strlen((char *)data);          //������Ч���ɳ���
	
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

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //�������ݵ�������

	return 0;
}

/*----------------------------------------------------------*/
/*�������������յ�����Ϣ���� �ȼ�1                          */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_Deal_Qs1(u8 *data)
{
	u8 data_len;                //�����ܳ���
	u16 topic_len;              //topic���Ƴ���
	u8 temp_topic[EMQTT_UNIT];  //���topic���Ƶ���ʱ������
	u8 temp_buff[EMQTT_UNIT];	//��ʱ������	
	u8 temp[4];                 //��ʱ������
	
	data_len = data[0];                     //��ȡ�����ܳ���
	
	topic_len  = data[3]*256;               //��ȡtopic���Ƴ���
	topic_len += data[4];	
	
	memcpy(temp_topic,&data[5],topic_len);                                     //topic���ƿ�������
	memcpy(temp_buff,&data[5+topic_len +2],data_len - 2 - 2 - 2 - topic_len);  //���ݿ�������
	
	temp[0]=0x40; 
	temp[1]=0x02;
	temp[2]=data[5+topic_len];
	temp[3]=data[6+topic_len];
	TxDataBuf_Deal(temp, 4);    //�������ݵ�������,�ȼ�1�����ͣ�����Ҫ�ָ�����������ʾ���յ�������

	if(!memcmp(temp_topic,Cntr_CMD,strlen(Cntr_CMD))) //���topic��Cntr_CMD��˵���ǿ��ƶ˷��͵�����
	{                     
		CMDBuf_Deal(temp_buff, data_len - 2 - 2 - 2 - topic_len);   //�������������
	}
	else  //���topic����Cntr_CMD��˵�����ǿ��ƶ˷��͵����ݣ����ݶ���
	{
		return 1;     
	}
	
	return 0;
}

/*----------------------------------------------------------*/
/*������������ȼ�1���ͱ��Ļظ�                             */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISHQs1_ACK(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD  *temPtr;             //��ʱMessageOBD��ָ��
	MessageOBD  *temPrevPtr;         //��ʱMessageOBD��ָ��
	
	temp_id =  data[3]*256;          //���㱨�ı�ʶ��
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 1;
	
	temPtr = First_MessageOBDPtr;      //ָ������ͷ;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //�жϱ��ı�ʶ���Ƿ����
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
			temPtr = temPtr->NextPtr;  //��һ��
			temp = 2;
		}
	}
	
	return temp;
}

/*----------------------------------------------------------*/
/*���������ȼ�2 ������Ϣ����                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBLISH_Qs2(char *topic_name, u8 *data)
{
    u16 Fixed_len;              //�̶���ͷ����
	u16 Variable_len;           //�ɱ䱨ͷ����
	u16 Payload_len;            //��Ч���ɳ���
    u8  temp_buff[EMQTT_UNIT];	//��ʱ������
	MessageOBD *temPtr;         //��ʱMessageOBD��ָ��
	
	if(topic_name == NULL)
	{
		EMQTT_PRINTF("topic���ƴ��󣬲���Ϊ��\r\n");
		return 1;
	}
	
	if(data == NULL)
	{
		EMQTT_PRINTF("���ݴ��󣬲���Ϊ��\r\n");
		return 2;
	}
	
	Fixed_len = 2;                               //�̶���ͷ����=2
	
	/*------2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 2�ֽڱ��ı�ʾ��*/
	Variable_len = 2 + strlen(topic_name) + 2;   //����ɱ䱨ͷ����
	
	/*------data�����ַ����ĳ���*/
	Payload_len = strlen((char *)data);          //������Ч���ɳ���
	
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

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //�������ݵ�������

	return 0;
}

/*----------------------------------------------------------*/
/*���������ȼ�2�����յ�                                     */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void EMQTT_PUBREC(u8 *data)
{
	u16 temp_id;                //��ű��ı�ʶ��
		
	temp_id =  data[3]*256;     //���㱨�ı�ʶ��
	temp_id += data[4];
	
    EMQTT_PUBREL(temp_id);      //���͵ȼ�2�����ͷű���	
}

/*----------------------------------------------------------*/
/*���������ȼ�2�����ͷ�                                     */
/*��  ����iD�����ı�ʾ��                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void EMQTT_PUBREL(u16 iD)
{
	u8 temp[4];                 //��ʱ������
		
	temp[0]=0x62; 
	temp[1]=0x02;
	temp[2]=iD/256;
	temp[3]=iD%256;
	TxDataBuf_Deal(temp, 4);    //�������ݵ�������,�ȼ�1�����ͣ�����Ҫ�ָ�����������ʾ���յ�������
}

/*----------------------------------------------------------*/
/*���������ȼ�2������ɱ���                                 */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PUBCOMP(u8 *data)
{
	u8  temp;
	u16 temp_id;
	MessageOBD  *temPtr;             //��ʱMessageOBD��ָ��
	MessageOBD  *temPrevPtr;         //��ʱMessageOBD��ָ��
	
	temp_id =  data[3]*256;          //���㱨�ı�ʶ��
	temp_id += data[4];
	
	if(First_MessageOBDPtr == NULL) return 1;
	
	temPtr = First_MessageOBDPtr;      //ָ������ͷ;
	while(temPtr != NULL)
	{
		if(temp_id == temPtr->iD)      //�жϱ��ı�ʶ���Ƿ����
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
			temPtr = temPtr->NextPtr;  //��һ��
			temp = 2;
		}
	}
	
	return temp;
}

/*----------------------------------------------------------*/
/*�������������յ�����Ϣ���� �ȼ�2                          */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_Deal_Qs2(u8 *data)
{
	u8 data_len;                //�����ܳ���
	u16 topic_len;              //topic���Ƴ���
	u8 temp_topic[EMQTT_UNIT];  //���topic���Ƶ���ʱ������
	u8 temp_buff[EMQTT_UNIT];	//��ʱ������	
	u8 temp[4];                 //��ʱ������
	
	data_len = data[0];                     //��ȡ�����ܳ���
	
	topic_len  = data[3]*256;               //��ȡtopic���Ƴ���
	topic_len += data[4];	
	
	memcpy(temp_topic,&data[5],topic_len);                                     //topic���ƿ�������
	memcpy(temp_buff,&data[5+topic_len +2],data_len - 2 - 2 - 2 - topic_len);  //���ݿ�������
	
	temp[0]=0x50; 
	temp[1]=0x02;
	temp[2]=data[5+topic_len];
	temp[3]=data[6+topic_len];
	TxDataBuf_Deal(temp, 4);    //�������ݵ�������,�ȼ�2�����ͣ�����Ҫ�ظ�����������ʾ���յ�������

	if(!memcmp(temp_topic,Cntr_CMD,strlen(Cntr_CMD))) //���topic��Cntr_CMD��˵���ǿ��ƶ˷��͵�����
	{                     
		CMDBuf_Deal(temp_buff, data_len - 2 - 2 - 2 - topic_len);   //�������������
	}
	else  //���topic����Cntr_CMD��˵�����ǿ��ƶ˷��͵����ݣ����ݶ���
	{
		return 1;     
	}
	
	return 0;
}

/*----------------------------------------------------------*/
/*���������ȼ�2�ظ��������                                 */
/*��  ����data������                                        */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void EMQTT_DealQs2_ACK(u8 *data)
{
	u8 temp[4];                //������
		
	temp[0]=0x70; 
	temp[1]=0x02;
	temp[2]=data[3];
	temp[3]=data[4];
	
	TxDataBuf_Deal(temp, 4);    //�������ݵ�������,�ȼ�1�����ͣ�����Ҫ�ָ�����������ʾ���յ�������
}

/*----------------------------------------------------------*/
/*��������PING���������ģ�������                            */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void EMQTT_PINGREQ(void)
{
	u8  temp_buff[2]={0xC0, 0x00};	//PING��������

	TxDataBuf_Deal(temp_buff, 2);   //�������ݵ�������
}

/*----------------------------------------------------------*/
/*��������PING��Ӧ���ģ��������Ļظ�                        */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
u8 EMQTT_PINGRESP(u8 *data)
{
	if(data[2]!=0x00) return 1;
 
    return 0;	
}

/*----------------------------------------------------------*/
/*���������Ͽ����ӱ���                                      */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void EMQTT_DISCONNECT(void)
{
	u8  temp_buff[2]={0xE0, 0x00};	//�Ͽ����ӱ�������

	TxDataBuf_Deal(temp_buff, 2);   //�������ݵ�������
}

/*----------------------------------------------------------*/
/*�������������ͻ�����                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(u8 *data, u8 size)
{
	memcpy(&EMQTT_TxDataInPtr[1],data,size);      //�������ݵ����ͻ�����
	EMQTT_TxDataInPtr[0] = size;                  //��¼���ݳ���
	EMQTT_TxDataInPtr+=EMQTT_UNIT;                //ָ������
	if(EMQTT_TxDataInPtr==EMQTT_TxDataEndPtr)     //���ָ�뵽������β����
		EMQTT_TxDataInPtr = EMQTT_TxDataBuf[0];   //ָ���λ����������ͷ
}

/*----------------------------------------------------------*/
/*����������������                                          */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void EMQTT_TxData(void)
{
	char temp_2[50]={0};
	
	SIM800C_TxData(EMQTT_TxDataOutPtr);
	neul_bc95_str_to_hex((const unsigned char *)EMQTT_TxDataOutPtr,strlen((const char *)EMQTT_TxDataOutPtr),temp_2);
	UartWriteBuffer_Config((u8 *)temp_2,strlen((char *)temp_2));	
}

/*----------------------------------------------------------*/
/*�������������������                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(u8 *data, u8 size)
{
	memcpy(&EMQTT_CMDInPtr[1],data,size);      //�������ݵ��������
	EMQTT_CMDInPtr[0] = size;                  //��¼���ݳ���
	EMQTT_CMDInPtr[size+1] = '\0';             //���������
	EMQTT_CMDInPtr+=EMQTT_UNIT;                //ָ������
	if(EMQTT_CMDInPtr==EMQTT_CMDEndPtr)        //���ָ�뵽������β����
		EMQTT_CMDInPtr = EMQTT_CMDBuf[0];      //ָ���λ����������ͷ
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

/*------------- ���ַ������м��ȡn���ַ� --------------*/
//char * mid(char *dst,char *src, int nLen,int nPos)
//{
//    char *p = src;
//    char *q = dst;
//    int len = strlen(src);
//    if(nLen>len) nLen = len-nPos;    /*�ӵ�nPos�������*/
//    if(nPos<0) nPos=0;    /*�ӵ�һ����ʼ*/
//    if(nPos>len) return NULL;
//    p += nPos;
//    while(nLen--) *(q++) = *(p++);
//    *(q++)='\0'; /*�б�Ҫ��?���б�Ҫ*/
//    return dst;
//}
