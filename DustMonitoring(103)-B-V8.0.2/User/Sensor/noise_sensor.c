#include "bsp.h"

//485��ȡNoise ��ʪ��
void getNoise(void){
	uint8_t read_count = 0;
	uint8_t u8MBStatus = 0;
	do{
		ModbusMaster_clearResponseBuffer();//��Buff
		u8MBStatus = ModbusMaster_readHoldingRegisters(NOISE_SENSOR_ADDR, 0x0000, 0x0004);
	}while((u8MBStatus != 0x00) && (++read_count < READ_RS485_COUNTS));
	
	if(read_count < READ_RS485_COUNTS && u8MBStatus == 0x00){ //��ȡ�ɹ�ȫ�ֱ�����ֵ
		mySensorDatas.NOISE = ModbusMaster_getResponseBuffer(0x00)/10;//Noise
		mySensorDatas.TEMP  = ModbusMaster_getResponseBuffer(0x01);//�¶�
		mySensorDatas.HUMI  = ModbusMaster_getResponseBuffer(0x02);//ʪ��		
		mySensorDatas.PRESS = ModbusMaster_getResponseBuffer(0x03);//��ѹ
	}else{
		printf("getWindDirection Error!\r\n");
	}
}

