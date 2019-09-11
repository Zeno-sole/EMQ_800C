#ifndef __GPRS_H
#define __GPRS_H

#include "bsp.h"

typedef enum{
    GPRS_TRUE,
    GPRS_FALSE,
}gprs_res_e;

#define     GPRS_CLEAN_RX()              clean_rebuff()
#define     GPRS_TX(cmd)                 GPRS_USART_printf("%s",cmd)
#define     GPRS_RX(len)                 ((char *)get_rebuff(&(len)))
#define     GPRS_DELAY(time)             BSP_OS_TimeDly(time) //��ʱ
#define     GPRS_SWAP16(data)    				 __REVSH(data)

uint8_t Send_AT_Command(char *cmd, char *reply, uint32_t waittime);

uint8_t Check_ACK(char *reply);

void GPRS_Connect(void);//GPRS����

uint8_t GPRS_Init(void);//��ʼ�������ģ��

uint8_t IsInsertCard(void);//���SIM���Ƿ����

uint8_t GSM_GPRS_init(void);//GPRS��ʼ������

uint8_t GPRS_TCPLink(char *localport, char * serverip, char * serverport);//TCP����

uint8_t GPRS_UDPLink(char *localport, char * serverip, char * serverport);//UDP����

uint8_t GPRS_Send(const char * str);//��������

uint8_t GPRS_LinkClose(void);//IP���ӶϿ�
uint8_t GPRS_ShutClose(void);//�رճ���

void GPRS_PWR(uint16_t sec);

void GPRS_Poll(void);//��ѯ����GPRS����
void HeartPack(void);//���������
void UploadData(void);//�ϴ�����������
#endif
