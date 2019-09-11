/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct SensorDataType mySensorDatas;//ȫ������
uint8_t modeSeq[5];//ִ����ʾ����
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
		BSP_InitUart_Config();//��ʼ�����ô���
//		GPRS_USART_Init();//��ʼ��GPRS����
		LED_UART_Init();
		PM25_UART3_Init();
//		Config_Init(); //��ϵͳ����
		Mode_Init();
		EMQ_Init(); //MQTT��ʼ��	

		Close_Measurement();  //�رղ���
		Open_Measurement();	  //��������
	}
}

static void InitGlobalVariables(void){
	DEVICE_ID = 3604;
	Mode = 0;
	memset(&mySensorDatas, 0, sizeof(struct SensorDataType));
}

/**
  * @brief  ϵͳ����
  * @param  None.
  * @retval None.
  */
static void Config_Init(void){
	
	uint8_t i;
	uint8_t readbuff[12];
	char number[5];
	
	memset(readbuff, 0 ,sizeof(readbuff));
	memset(number, 0 ,sizeof(number));

	FLASH_ReadHalfWord(readbuff, sizeof(readbuff));//��������

	for(i=0; i< (sizeof(readbuff) - 1); i++){
		
		if(readbuff[i] == '\r' && readbuff[i+1] == '\n'){
			
			if(i < 5){ //����С��5λ��
				memcpy(number, &readbuff[0], i);
				DEVICE_ID = strtol(number, NULL, 10);
				printf("DEVICE_ID:%d\r\n", DEVICE_ID);
			}
			break;
		}
	}
}

/**
  * @brief  ��ʾģʽ��ʼ��
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
  * @brief  �����λ.
  * @param  None.
  * @retval None.
  */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* �ر������ж�*/  
  NVIC_SystemReset();   /* ϵͳ��λ */
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
	FLASH_Unlock();/* Flash������������� */
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);/* ������б�� */	
	FLASHStatus = FLASH_ErasePage(FLASH_BASE_ADDR + (FLASH_PAGE_SIZE * 255)); /* ����FLASH ��255ҳ */
	if(FLASHStatus == FLASH_COMPLETE)
	{
		while(len--){
			FLASHStatus = FLASH_ProgramHalfWord((FLASH_START_ADDR + WORD_WIDE * i), *pBuffer++);/* Flash��� */
			i++;
		}	
	}
	FLASH_Lock();  /* �������ݴ洢�� */
}
void delay_ms(u16 num){

	BSP_OS_TimeDlyMs(num);

}


