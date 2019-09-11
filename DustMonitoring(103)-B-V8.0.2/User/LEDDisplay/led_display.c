#include  "bsp.h"

const uint8_t *level[11]={"�缶: �㼶��", "�缶: һ����", "�缶: ������", "�缶: ������", "�缶: �ļ���", "�缶: �弶��", "�缶: ������", "�缶: �߼���", "�缶: �˼���", "�缶: �ż���", "�缶: ʮ����"};

//���ֽ���ǰ
uint8_t display_template[64] = {
	0x55, 0xAA, 0x00, 0x00, //֡ͷ
	0x01,   //��ַ
	0x01,   //��־
	0x00, 0xD9, //������
	0x00, 0x00, //����
	0x00, 0x00, 0x00, 0x00, //֡���
	0x00, 0x00, 0x00, 0x00, //�ܳ���
	0x00, 0x00, //֡����

	0x25, 0x64, 0x69, 0x73, 0x70, //%disp��ʾ����Э��ͷ��
	0x30, //��������0x30��ʾ��һ��
	0x3a, //��ʾ��ɫ
	0x30, //��Ч
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //indexΪ28-39��ʼ�ı�����
	
	0x25, 0x64, 0x69, 0x73, 0x70, //%disp��ʾ����Э��ͷ��
	0x31, //��������0x31��ʾ�ڶ���
	0x3a, //��ʾ��ɫ
	0x30, //��Ч
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //indexΪ48-59�ı�����
	
	0x00, 0x00, 0x0D, 0x0A  //֡β
};//�����ۼӺ�Ϊ:1744

static void SendLEDCmd(const char *h1_data, const char *h2_data);
static uint8_t CheckDisplayData(const uint8_t *pBuffer, uint32_t NumBytes);
static void dispWindLevelAndWelcome(uint16_t param);
static void dispWindSpeedAndWindDirect(uint16_t param);
static void dispTempAndHumi(uint16_t param);
static void dispNoiseAndTSP(uint16_t param);
static void dispPM25AndPM10(uint16_t param1);

typedef void(*FUN)(uint16_t param);

//��ʾ����
FUN _class[5]={
	dispWindLevelAndWelcome,
	dispWindSpeedAndWindDirect,
	dispTempAndHumi,
	dispNoiseAndTSP,
	dispPM25AndPM10
};

static void SendLEDCmd(const char *h1_data, const char *h2_data)
{
	memcpy(&display_template[28], h1_data, 12);
	memcpy(&display_template[48], h2_data, 12);
	//����LED��ʾ����ǰУ������
	if(CheckDisplayData(display_template, sizeof(display_template)) == 0){
		USART1WriteBuffer(display_template, sizeof(display_template));
	}
}

/**
 * @brief  �����ʾ�����Ƿ���ȷ
 * @param  ��
 * @retval ��
 */
static uint8_t CheckDisplayData(const uint8_t *pBuffer, uint32_t NumBytes){
	int rc = 0;
	uint16_t total = 0;
	uint8_t i;
	uint8_t temp[64];
	memset(temp, 0, sizeof(temp));
	memcpy(temp, pBuffer, NumBytes);//����
	memset(&temp[28], 0, 12);//����ɱ�����
	memset(&temp[48], 0, 12);//����ɱ�����
	for(i=0; i<NumBytes; i++){
		total += temp[i];//����
	}
	//���̶������ۼӺ��Ƿ�Ϊ1744
	if(total != 1744){
		rc = -1;
	}
	return rc;
}

void DisplayNumberMode(void){

	char temp0[13] = {0};
	char temp1[13] = {0};
	sprintf(temp0, "���: %-4d", DEVICE_ID);
	sprintf(temp1, "ģʽ: %-6d", Mode);
	SendLEDCmd(temp0, temp1);
}

static void dispWindLevelAndWelcome(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
	getWindSpeed();//��ȡ����
	
	sprintf(temp0, "%s", level[mySensorDatas.WIND_LEVEL]);
	sprintf(temp1, "��ѹ:%4dKPa",mySensorDatas.PRESS);
	SendLEDCmd(temp0, temp1);
}

static void dispWindSpeedAndWindDirect(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
	getWindDirection();//��ȡ����
	
	sprintf(temp0, "����:%4.1fm/s", (float)mySensorDatas.WIND_SPEED/10);
	//����
	if(( mySensorDatas.WIND_DIRECTION >=337 && mySensorDatas.WIND_DIRECTION <= 360 ) || ( mySensorDatas.WIND_DIRECTION == 0 || mySensorDatas.WIND_DIRECTION <22))
	{
		mySensorDatas.WIND_DIRECTION_CLASS=1;
		sprintf(temp1, "%s", "����: ������");
	}
	//����
	else if( mySensorDatas.WIND_DIRECTION >= 22 && mySensorDatas.WIND_DIRECTION <67 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=2;
		sprintf(temp1, "%s", "����: ������");
	}
	//����
	else if( mySensorDatas.WIND_DIRECTION >= 67 && mySensorDatas.WIND_DIRECTION <112 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=3;
		sprintf(temp1, "%s", "����: ������");
	}
	//����
	else if( mySensorDatas.WIND_DIRECTION >= 112 && mySensorDatas.WIND_DIRECTION <157 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=4;
		sprintf(temp1, "%s", "����: ���Ϸ�");
	}
	//����
	else if( mySensorDatas.WIND_DIRECTION >= 157 && mySensorDatas.WIND_DIRECTION <202 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=5;
		sprintf(temp1, "%s", "����: ���Ϸ�");
	}
	//����
	else if( mySensorDatas.WIND_DIRECTION >= 202 && mySensorDatas.WIND_DIRECTION < 247 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=6;
		sprintf(temp1, "%s", "����: ���Ϸ�");
	}
	//����
	else if( mySensorDatas.WIND_DIRECTION >= 247 && mySensorDatas.WIND_DIRECTION < 292 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=7;
		sprintf(temp1, "%s", "����: ������");
	}
	//����
	else if( mySensorDatas.WIND_DIRECTION >= 292 && mySensorDatas.WIND_DIRECTION < 337 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=8;
		sprintf(temp1, "%s", "����: ������");
	}
	else
	{
		sprintf(temp1, "%s", "����:       ");
	}
	
	SendLEDCmd(temp0, temp1);
}

static void dispTempAndHumi(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
		getNoise();//��ȡ�¶�/ʪ��
	
	sprintf(temp0, "�¶�:%5.1f��",(float)mySensorDatas.TEMP/100);
	sprintf(temp1, "ʪ��:%5.1f%%",(float)mySensorDatas.HUMI/100);
	SendLEDCmd(temp0, temp1);
}

static void dispNoiseAndTSP(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
	getNoise();//��ȡ����
	ParsePM25Command();//��ȡTSP
	
	sprintf(temp0, "����:%5ddB", mySensorDatas.NOISE);
	sprintf(temp1, "TSP:%6dug", mySensorDatas.TSP);
	SendLEDCmd(temp0, temp1);
}

static void dispPM25AndPM10(uint16_t param1){
	char temp0[13] = {0};
	char temp1[13] = {0};

	ParsePM25Command();//��ȡ�¶�/ʪ��/PM2.5/PM10/TSP

	sprintf(temp0, "PM2.5:%4dug", mySensorDatas.PM2_5);
	sprintf(temp1, "PM10:%5dug", mySensorDatas.PM10);

	SendLEDCmd(temp0, temp1);

	//PM10����ֵ����JDQ
	if(mySensorDatas.PM10 >= 150){
		RELAY_PORT->ODR &= ~(RELAY_PIN);//Relay Off
	}else{
		RELAY_PORT->ODR |= RELAY_PIN;//Relay On
	}
}


void UpdateDisplay(void){
	OS_ERR  os_err;
	uint8_t i;
	for(i=0; i< sizeof(modeSeq); i++){
		if(modeSeq[i] == DISPLAY_DISABLE){
			break;
		}else{
			(_class[modeSeq[i] - 1])(0);//ִ����ʾ����
			OSTimeDlyHMSM(0u, 0u, 3u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);//��ʱ3��
		}
	}
}

