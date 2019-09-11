#include  "bsp.h"

const uint8_t *level[11]={"风级: 零级风", "风级: 一级风", "风级: 二级风", "风级: 三级风", "风级: 四级风", "风级: 五级风", "风级: 六级风", "风级: 七级风", "风级: 八级风", "风级: 九级风", "风级: 十级风"};

//低字节在前
uint8_t display_template[64] = {
	0x55, 0xAA, 0x00, 0x00, //帧头
	0x01,   //地址
	0x01,   //标志
	0x00, 0xD9, //操作码
	0x00, 0x00, //保留
	0x00, 0x00, 0x00, 0x00, //帧序号
	0x00, 0x00, 0x00, 0x00, //总长度
	0x00, 0x00, //帧长度

	0x25, 0x64, 0x69, 0x73, 0x70, //%disp显示内容协议头部
	0x30, //区域码编号0x30显示第一行
	0x3a, //显示颜色
	0x30, //特效
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //index为28-39开始文本内容
	
	0x25, 0x64, 0x69, 0x73, 0x70, //%disp显示内容协议头部
	0x31, //区域码编号0x31显示第二行
	0x3a, //显示颜色
	0x30, //特效
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //index为48-59文本内容
	
	0x00, 0x00, 0x0D, 0x0A  //帧尾
};//数据累加和为:1744

static void SendLEDCmd(const char *h1_data, const char *h2_data);
static uint8_t CheckDisplayData(const uint8_t *pBuffer, uint32_t NumBytes);
static void dispWindLevelAndWelcome(uint16_t param);
static void dispWindSpeedAndWindDirect(uint16_t param);
static void dispTempAndHumi(uint16_t param);
static void dispNoiseAndTSP(uint16_t param);
static void dispPM25AndPM10(uint16_t param1);

typedef void(*FUN)(uint16_t param);

//显示序列
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
	//更新LED显示数据前校验数据
	if(CheckDisplayData(display_template, sizeof(display_template)) == 0){
		USART1WriteBuffer(display_template, sizeof(display_template));
	}
}

/**
 * @brief  检查显示数据是否正确
 * @param  无
 * @retval 无
 */
static uint8_t CheckDisplayData(const uint8_t *pBuffer, uint32_t NumBytes){
	int rc = 0;
	uint16_t total = 0;
	uint8_t i;
	uint8_t temp[64];
	memset(temp, 0, sizeof(temp));
	memcpy(temp, pBuffer, NumBytes);//副本
	memset(&temp[28], 0, 12);//清除可变数据
	memset(&temp[48], 0, 12);//清除可变数据
	for(i=0; i<NumBytes; i++){
		total += temp[i];//加总
	}
	//检查固定数据累加和是否为1744
	if(total != 1744){
		rc = -1;
	}
	return rc;
}

void DisplayNumberMode(void){

	char temp0[13] = {0};
	char temp1[13] = {0};
	sprintf(temp0, "编号: %-4d", DEVICE_ID);
	sprintf(temp1, "模式: %-6d", Mode);
	SendLEDCmd(temp0, temp1);
}

static void dispWindLevelAndWelcome(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
	getWindSpeed();//获取风速
	
	sprintf(temp0, "%s", level[mySensorDatas.WIND_LEVEL]);
	sprintf(temp1, "气压:%4dKPa",mySensorDatas.PRESS);
	SendLEDCmd(temp0, temp1);
}

static void dispWindSpeedAndWindDirect(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
	getWindDirection();//获取风向
	
	sprintf(temp0, "风速:%4.1fm/s", (float)mySensorDatas.WIND_SPEED/10);
	//正北
	if(( mySensorDatas.WIND_DIRECTION >=337 && mySensorDatas.WIND_DIRECTION <= 360 ) || ( mySensorDatas.WIND_DIRECTION == 0 || mySensorDatas.WIND_DIRECTION <22))
	{
		mySensorDatas.WIND_DIRECTION_CLASS=1;
		sprintf(temp1, "%s", "风向: 正北风");
	}
	//东北
	else if( mySensorDatas.WIND_DIRECTION >= 22 && mySensorDatas.WIND_DIRECTION <67 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=2;
		sprintf(temp1, "%s", "风向: 东北风");
	}
	//正东
	else if( mySensorDatas.WIND_DIRECTION >= 67 && mySensorDatas.WIND_DIRECTION <112 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=3;
		sprintf(temp1, "%s", "风向: 正东风");
	}
	//东南
	else if( mySensorDatas.WIND_DIRECTION >= 112 && mySensorDatas.WIND_DIRECTION <157 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=4;
		sprintf(temp1, "%s", "风向: 东南风");
	}
	//正南
	else if( mySensorDatas.WIND_DIRECTION >= 157 && mySensorDatas.WIND_DIRECTION <202 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=5;
		sprintf(temp1, "%s", "风向: 正南风");
	}
	//西南
	else if( mySensorDatas.WIND_DIRECTION >= 202 && mySensorDatas.WIND_DIRECTION < 247 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=6;
		sprintf(temp1, "%s", "风向: 西南风");
	}
	//正西
	else if( mySensorDatas.WIND_DIRECTION >= 247 && mySensorDatas.WIND_DIRECTION < 292 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=7;
		sprintf(temp1, "%s", "风向: 正西风");
	}
	//西北
	else if( mySensorDatas.WIND_DIRECTION >= 292 && mySensorDatas.WIND_DIRECTION < 337 )
	{
		mySensorDatas.WIND_DIRECTION_CLASS=8;
		sprintf(temp1, "%s", "风向: 西北风");
	}
	else
	{
		sprintf(temp1, "%s", "风向:       ");
	}
	
	SendLEDCmd(temp0, temp1);
}

static void dispTempAndHumi(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
		getNoise();//获取温度/湿度
	
	sprintf(temp0, "温度:%5.1f℃",(float)mySensorDatas.TEMP/100);
	sprintf(temp1, "湿度:%5.1f%%",(float)mySensorDatas.HUMI/100);
	SendLEDCmd(temp0, temp1);
}

static void dispNoiseAndTSP(uint16_t param){
	char temp0[13] = {0};
	char temp1[13] = {0};
	
	getNoise();//获取噪声
	ParsePM25Command();//获取TSP
	
	sprintf(temp0, "噪音:%5ddB", mySensorDatas.NOISE);
	sprintf(temp1, "TSP:%6dug", mySensorDatas.TSP);
	SendLEDCmd(temp0, temp1);
}

static void dispPM25AndPM10(uint16_t param1){
	char temp0[13] = {0};
	char temp1[13] = {0};

	ParsePM25Command();//获取温度/湿度/PM2.5/PM10/TSP

	sprintf(temp0, "PM2.5:%4dug", mySensorDatas.PM2_5);
	sprintf(temp1, "PM10:%5dug", mySensorDatas.PM10);

	SendLEDCmd(temp0, temp1);

	//PM10超过值控制JDQ
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
			(_class[modeSeq[i] - 1])(0);//执行显示序列
			OSTimeDlyHMSM(0u, 0u, 3u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);//延时3秒
		}
	}
}

