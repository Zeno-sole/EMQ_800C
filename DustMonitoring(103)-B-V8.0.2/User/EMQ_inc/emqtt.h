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
	struct  Message_OBD *NextPtr;                      //指向下一个MessageOBD结构体
	u8                   MessageOBDBuf[EMQTT_UNIT];    //存放报文数据
	u16                  data_len;                     //存放的报文数据的大小
	u16                  iD;                           //报文标识符
}MessageOBD;

extern u8  EMQTT_RxDataBuf[EMQTT_NUM][EMQTT_UNIT];   //数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern u8 *EMQTT_RxDataInPtr;                        //指向缓冲区存放数据的位置
extern u8 *EMQTT_RxDataOutPtr;                       //指向缓冲区读取数据的位置
extern u8 *EMQTT_RxDataEndPtr;                       //指向缓冲区结束的位置

extern u8  EMQTT_TxDataBuf[EMQTT_NUM][EMQTT_UNIT];   //数据的发送缓冲区,所有发往服务器的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern u8 *EMQTT_TxDataInPtr;                        //指向缓冲区存放数据的位置
extern u8 *EMQTT_TxDataOutPtr;                       //指向缓冲区读取数据的位置
extern u8 *EMQTT_TxDataEndPtr;                       //指向缓冲区结束的位置

extern u8  EMQTT_CMDBuf[CMD_NUM][EMQTT_UNIT];        //命令数据的接收缓冲区
extern u8 *EMQTT_CMDInPtr;                           //指向缓冲区存放数据的位置
extern u8 *EMQTT_CMDOutPtr;                          //指向缓冲区读取数据的位置
extern u8 *EMQTT_CMDEndPtr;                          //指向缓冲区结束的位置

extern MessageOBD    *First_MessageOBDPtr;           //被占用的MessageOBD结构体链表头的指针
extern char *Cntr_CMD;                               //订阅控制端的topic名称
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
