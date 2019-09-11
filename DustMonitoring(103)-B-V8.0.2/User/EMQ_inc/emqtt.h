/*-------------------------------------------------*/

/*-------------------------------------------------*/

#ifndef __EMQTT_H
#define __EMQTT_H

#include "BSP_EMQ.h"

#define  EMQTT_PRINTF        printf
#define  EMQTT_UNIT          50
#define  EMQTT_NUM           200
#define  MessageOBD_NUM      50
#define  CMD_NUM             50



typedef struct   Message_OBD
{
	struct  Message_OBD *NextPtr;                      //ָ����һ��MessageOBD�ṹ��
	u8                   MessageOBDBuf[EMQTT_UNIT];    //��ű�������
	u16                  data_len;                     //��ŵı������ݵĴ�С
	u16                  iD;                           //���ı�ʶ��
}MessageOBD;

extern u8  EMQTT_RxDataBuf[EMQTT_NUM][EMQTT_UNIT];   //���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern u8 *EMQTT_RxDataInPtr;                        //ָ�򻺳���������ݵ�λ��
extern u8 *EMQTT_RxDataOutPtr;                       //ָ�򻺳�����ȡ���ݵ�λ��
extern u8 *EMQTT_RxDataEndPtr;                       //ָ�򻺳���������λ��

extern u8  EMQTT_TxDataBuf[EMQTT_NUM][EMQTT_UNIT];   //���ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern u8 *EMQTT_TxDataInPtr;                        //ָ�򻺳���������ݵ�λ��
extern u8 *EMQTT_TxDataOutPtr;                       //ָ�򻺳�����ȡ���ݵ�λ��
extern u8 *EMQTT_TxDataEndPtr;                       //ָ�򻺳���������λ��

extern u8  EMQTT_CMDBuf[CMD_NUM][EMQTT_UNIT];        //�������ݵĽ��ջ�����
extern u8 *EMQTT_CMDInPtr;                           //ָ�򻺳���������ݵ�λ��
extern u8 *EMQTT_CMDOutPtr;                          //ָ�򻺳�����ȡ���ݵ�λ��
extern u8 *EMQTT_CMDEndPtr;                          //ָ�򻺳���������λ��

extern MessageOBD    *First_MessageOBDPtr;           //��ռ�õ�MessageOBD�ṹ������ͷ��ָ��
extern char *Cntr_CMD;                               //���Ŀ��ƶ˵�topic����
extern char *CLIENT_ID;
extern OS_TCB Task1_TaskTCB;
void EMQTT_Init(void);
u8 EMQTT_CONNECT(void);
u8 EMQTT_CONNACK(u8 *data);
u8 EMQTT_SUBSCRIBE(char *topic_name, u8 QoS);
u8 EMQTT_SUBACK(u8 *data);
u8 EMQTT_UNSUBSCRIBE(char *topic_name);
u8 EMQTT_UNSUBACK(u8 *data);
u8 EMQTT_PUBLISH_Qs0(char *topic_name, u8 *data);
u8 EMQTT_Deal_Qs0(u8 *data);
u8 EMQTT_PUBLISH_Qs1(char *topic_name, u8 *data);
u8 EMQTT_Deal_Qs1(u8 *data);
u8 EMQTT_PUBLISHQs1_ACK(u8 *data);
u8 EMQTT_PUBLISH_Qs2(char *topic_name, u8 *data);
void EMQTT_PUBREC(u8 *data);
void EMQTT_PUBREL(u16 iD);
u8 EMQTT_PUBCOMP(u8 *data);
u8 EMQTT_Deal_Qs2(u8 *data);
void EMQTT_DealQs2_ACK(u8 *data);
void EMQTT_PINGREQ(void);
u8 EMQTT_PINGRESP(u8 *data);
void EMQTT_DISCONNECT(void);
void TxDataBuf_Deal(u8 *data, u8 size);
void EMQTT_TxData(void);
void CMDBuf_Deal(u8 *data, u8 size);
void Restart_Link(void);

int neul_bc95_str_to_hex(const unsigned char *bufin, int len, char *bufout);
int neul_bc95_hex_to_str(const char *bufin, int len, char *bufout);
//char * mid(char *dst,char *src, int nLen,int nPos);
#endif
