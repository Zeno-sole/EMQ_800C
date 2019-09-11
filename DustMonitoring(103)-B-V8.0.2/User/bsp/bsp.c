/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct SensorDataType mySensorDatas;//全局数据
uint8_t modeSeq[5];//执行显示序列
//uint16_t DEVICE_ID;
uint8_t Mode;

static void InitGlobalVariables(void);
static void Config_Init(void);
static void Mode_Init(void);

void BSP_Init(void)
{
	if (OSRunning > 0u){
		BSP_Tick_Init();
		BSP_IntInit();
		InitGlobalVariables();
		BSP_GPIO_Init();
		BSP_InitUart_Config();//初始化配置串口
//		GPRS_USART_Init();//初始化GPRS串口
		LED_UART_Init();
		PM25_UART3_Init();
//		Config_Init(); //读系统配置
		Mode_Init();
		EMQ_Init(); //MQTT初始化	

		Close_Measurement();  //关闭测量
		Open_Measurement();	  //开启测量
	}
}

static void InitGlobalVariables(void){
	DEVICE_ID = 3604;
	Mode = 0;
	memset(&mySensorDatas, 0, sizeof(struct SensorDataType));
}

/**
  * @brief  系统配置
  * @param  None.
  * @retval None.
  */
static void Config_Init(void){
	
	uint8_t i;
	uint8_t readbuff[12];
	char number[5];
	
	memset(readbuff, 0 ,sizeof(readbuff));
	memset(number, 0 ,sizeof(number));

	FLASH_ReadHalfWord(readbuff, sizeof(readbuff));//读出数据

	for(i=0; i< (sizeof(readbuff) - 1); i++){
		
		if(readbuff[i] == '\r' && readbuff[i+1] == '\n'){
			
			if(i < 5){ //必须小于5位数
				memcpy(number, &readbuff[0], i);
				DEVICE_ID = strtol(number, NULL, 10);
				printf("DEVICE_ID:%d\r\n", DEVICE_ID);
			}
			break;
		}
	}
}

/**
  * @brief  显示模式初始化
  * @param  None.
  * @retval None.
  */
static void Mode_Init(void){
	if(!(MODE_SEL_PORT->IDR & MODE_SEL1_PIN)){
		modeSeq[0] = DISPLAY_PM25_PM10;
		modeSeq[1] = DISPLAY_DISABLE;
		modeSeq[2] = DISPLAY_DISABLE;
		modeSeq[3] = DISPLAY_DISABLE;
		modeSeq[4] = DISPLAY_DISABLE;
		Mode = 1;
		printf("Mode1\r\n");
	}else if(!(MODE_SEL_PORT->IDR & MODE_SEL2_PIN)){
		modeSeq[0] = DISPLAY_NOISE_TSP;
		modeSeq[1] = DISPLAY_PM25_PM10;
		modeSeq[2] = DISPLAY_DISABLE;
		modeSeq[3] = DISPLAY_DISABLE;
		modeSeq[4] = DISPLAY_DISABLE;
		Mode = 2;
		printf("Mode2\r\n");
	}else if(!(MODE_SEL_PORT->IDR & MODE_SEL3_PIN)){
		modeSeq[0] = DISPLAY_TEMP_HUMI;
		modeSeq[1] = DISPLAY_NOISE_TSP;
		modeSeq[2] = DISPLAY_PM25_PM10;
		modeSeq[3] = DISPLAY_DISABLE;
		modeSeq[4] = DISPLAY_DISABLE;
		Mode = 3;
		printf("Mode3\r\n");
	}else if(!(MODE_SEL_PORT->IDR & MODE_SEL4_PIN)){
		modeSeq[0] = DISPLAY_LEVEL_WELCOME;
		modeSeq[1] = DISPLAY_SPEED_DIRECT;
		modeSeq[2] = DISPLAY_TEMP_HUMI;
		modeSeq[3] = DISPLAY_NOISE_TSP;
		modeSeq[4] = DISPLAY_PM25_PM10;
		Mode = 4;
		printf("Mode4\r\n");
	}
}

/**
  * @brief  软件复位.
  * @param  None.
  * @retval None.
  */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* 关闭所有中断*/  
  NVIC_SystemReset();   /* 系统复位 */
}

void FLASH_ReadHalfWord(uint8_t *pBuffer, uint16_t len){
	
	uint16_t i = 0;
	if(len > 511)return;
	while(len--){
		*pBuffer++ = (*(uint16_t*)(FLASH_START_ADDR + WORD_WIDE * i));
		i++;
	}
}

void Flash_WriteHalfWord(const uint8_t *pBuffer, uint16_t len){
	
	uint16_t i = 0;
	FLASH_Status FLASHStatus;
	if(len > 511)return;
	FLASH_Unlock();/* Flash程序控制器解锁 */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);/* 清除所有标记 */	
	FLASHStatus = FLASH_ErasePage(FLASH_BASE_ADDR + (FLASH_PAGE_SIZE * 255)); /* 擦除FLASH 第255页 */
	if(FLASHStatus == FLASH_COMPLETE)
	{
		while(len--){
			FLASHStatus = FLASH_ProgramHalfWord((FLASH_START_ADDR + WORD_WIDE * i), *pBuffer++);/* Flash编程 */
			i++;
		}	
	}
	FLASH_Lock();  /* 锁定数据存储器 */
}
void delay_ms(u16 num){

	BSP_OS_TimeDlyMs(num);

}


