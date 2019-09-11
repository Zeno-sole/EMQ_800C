#include "bsp.h"

//485��ȡ����
void getWindSpeed(void){

	uint8_t read_count = 0;
	uint8_t u8MBStatus = 0;
	do{
		ModbusMaster_clearResponseBuffer();//��Buff
		u8MBStatus = ModbusMaster_readHoldingRegisters(WIND_SPEED_SENSOR_ADDR, 0x0000, 0x0001);
	}while((u8MBStatus != 0x00) && (++read_count < READ_RS485_COUNTS));

	if(read_count < READ_RS485_COUNTS && u8MBStatus == 0x00){ //��ȡ�ɹ�ȫ�ֱ�����ֵ
		float level = 0.0;
		mySensorDatas.WIND_SPEED = ModbusMaster_getResponseBuffer(0x00);
		level = (float)mySensorDatas.WIND_SPEED/10;
		if(level <= 0.2){ //0����(�޷�)
			mySensorDatas.WIND_LEVEL = 0;
		}else if(level >= 0.3 && level <= 1.6){ //1����(���)
			mySensorDatas.WIND_LEVEL = 1;
		}else if(level > 1.6 && level <= 3.4){ //2����(���)
			mySensorDatas.WIND_LEVEL = 2;
		}else if(level > 3.4 && level <= 5.5){ //3����(΢��)
			mySensorDatas.WIND_LEVEL = 3;
		}else if(level > 5.5 && level <= 8.0){ //4����(�ͷ�)
			mySensorDatas.WIND_LEVEL = 4;
		}else if(level > 8.0 && level <= 10.8){ //5����(���)
			mySensorDatas.WIND_LEVEL = 5;
		}else if(level > 10.8 && level <= 13.9){ //6����(ǿ��)
			mySensorDatas.WIND_LEVEL = 6;
		}else if(level > 13.9 && level <= 17.2){ //7����(����)
			mySensorDatas.WIND_LEVEL = 7;
		}else if(level > 17.2 && level <= 20.8){ //8����(���)
			mySensorDatas.WIND_LEVEL = 8;
		}else if(level > 20.8 && level <= 24.5){ //9����(�ҷ�)
			mySensorDatas.WIND_LEVEL = 9;
		}else if(level > 24.5 && level <= 28.5){ //10����(���)
			mySensorDatas.WIND_LEVEL = 10;
		}
	}else{
		printf("getWindSpeed Error!\r\n");
	}
}

//485��ȡ����
void getWindDirection(void){
	uint8_t read_count = 0;
	uint8_t u8MBStatus = 0;
	do{
		ModbusMaster_clearResponseBuffer();//��Buff
		u8MBStatus = ModbusMaster_readHoldingRegisters(WIND_DIRECTION_SENSOR_ADDR, 0x0000, 0x0001);
	}while((u8MBStatus != 0x00) && (++read_count < READ_RS485_COUNTS));
	
	if(read_count < READ_RS485_COUNTS && u8MBStatus == 0x00){ //��ȡ�ɹ�ȫ�ֱ�����ֵ
		mySensorDatas.WIND_DIRECTION = ModbusMaster_getResponseBuffer(0x00);
	}else{
		printf("getWindDirection Error!\r\n");
	}
}

