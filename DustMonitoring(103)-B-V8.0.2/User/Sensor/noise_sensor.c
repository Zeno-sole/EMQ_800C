#include "bsp.h"

//485获取Noise 温湿度
void getNoise(void){
	uint8_t read_count = 0;
	uint8_t u8MBStatus = 0;
	do{
		ModbusMaster_clearResponseBuffer();//清Buff
		u8MBStatus = ModbusMaster_readHoldingRegisters(NOISE_SENSOR_ADDR, 0x0000, 0x0004);
	}while((u8MBStatus != 0x00) && (++read_count < READ_RS485_COUNTS));
	
	if(read_count < READ_RS485_COUNTS && u8MBStatus == 0x00){ //获取成功全局变量赋值
		mySensorDatas.NOISE = ModbusMaster_getResponseBuffer(0x00)/10;//Noise
		mySensorDatas.TEMP  = ModbusMaster_getResponseBuffer(0x01);//温度
		mySensorDatas.HUMI  = ModbusMaster_getResponseBuffer(0x02);//湿度		
		mySensorDatas.PRESS = ModbusMaster_getResponseBuffer(0x03);//气压
	}else{
		printf("getWindDirection Error!\r\n");
	}
}

