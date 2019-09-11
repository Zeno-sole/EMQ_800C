#include "bsp.h"

//485获取风速
void getWindSpeed(void){

	uint8_t read_count = 0;
	uint8_t u8MBStatus = 0;
	do{
		ModbusMaster_clearResponseBuffer();//清Buff
		u8MBStatus = ModbusMaster_readHoldingRegisters(WIND_SPEED_SENSOR_ADDR, 0x0000, 0x0001);
	}while((u8MBStatus != 0x00) && (++read_count < READ_RS485_COUNTS));

	if(read_count < READ_RS485_COUNTS && u8MBStatus == 0x00){ //获取成功全局变量赋值
		float level = 0.0;
		mySensorDatas.WIND_SPEED = ModbusMaster_getResponseBuffer(0x00);
		level = (float)mySensorDatas.WIND_SPEED/10;
		if(level <= 0.2){ //0级风(无风)
			mySensorDatas.WIND_LEVEL = 0;
		}else if(level >= 0.3 && level <= 1.6){ //1级风(软风)
			mySensorDatas.WIND_LEVEL = 1;
		}else if(level > 1.6 && level <= 3.4){ //2级风(轻风)
			mySensorDatas.WIND_LEVEL = 2;
		}else if(level > 3.4 && level <= 5.5){ //3级风(微风)
			mySensorDatas.WIND_LEVEL = 3;
		}else if(level > 5.5 && level <= 8.0){ //4级风(和风)
			mySensorDatas.WIND_LEVEL = 4;
		}else if(level > 8.0 && level <= 10.8){ //5级风(清风)
			mySensorDatas.WIND_LEVEL = 5;
		}else if(level > 10.8 && level <= 13.9){ //6级风(强风)
			mySensorDatas.WIND_LEVEL = 6;
		}else if(level > 13.9 && level <= 17.2){ //7级风(劲风)
			mySensorDatas.WIND_LEVEL = 7;
		}else if(level > 17.2 && level <= 20.8){ //8级风(大风)
			mySensorDatas.WIND_LEVEL = 8;
		}else if(level > 20.8 && level <= 24.5){ //9级风(烈风)
			mySensorDatas.WIND_LEVEL = 9;
		}else if(level > 24.5 && level <= 28.5){ //10级风(狂风)
			mySensorDatas.WIND_LEVEL = 10;
		}
	}else{
		printf("getWindSpeed Error!\r\n");
	}
}

//485获取风向
void getWindDirection(void){
	uint8_t read_count = 0;
	uint8_t u8MBStatus = 0;
	do{
		ModbusMaster_clearResponseBuffer();//清Buff
		u8MBStatus = ModbusMaster_readHoldingRegisters(WIND_DIRECTION_SENSOR_ADDR, 0x0000, 0x0001);
	}while((u8MBStatus != 0x00) && (++read_count < READ_RS485_COUNTS));
	
	if(read_count < READ_RS485_COUNTS && u8MBStatus == 0x00){ //获取成功全局变量赋值
		mySensorDatas.WIND_DIRECTION = ModbusMaster_getResponseBuffer(0x00);
	}else{
		printf("getWindDirection Error!\r\n");
	}
}

