#include  "bsp.h"

static void BSP_GPIO_Config(void);

void BSP_GPIO_Init(void){
	BSP_GPIO_Config();
}

static void BSP_GPIO_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);/* Enable GPIO clock */
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);/*Disable the JTAG*/
	
	/* Configure RS485 RDEN for PA15 Pin  */
	GPIO_InitStructure.GPIO_Pin = RS485_RDEN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RS485_RDEN_PORT, &GPIO_InitStructure);
	RS485_RDEN_PORT->ODR &= ~(RS485_RDEN_PIN);//Low Receiver
	//RS485_RDEN_PORT->ODR |= RS485_RDEN_PIN;//High Output
	
	/* Configure Link LED for PD9 Pin  */
	GPIO_InitStructure.GPIO_Pin = (LINK_LED_PIN);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LINK_LED_PORT, &GPIO_InitStructure);
	LINK_LED_PORT->ODR &= ~(LINK_LED_PIN);//LINK LED Off
	
	/* Configure Relay for PE0 Pin  */
	GPIO_InitStructure.GPIO_Pin = (RELAY_PIN);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RELAY_PORT, &GPIO_InitStructure);
	RELAY_PORT->ODR |= (RELAY_PIN);//Relay Off
	
	/* Configure GPRS PWR for PE3 Pin  */
	GPIO_InitStructure.GPIO_Pin = (GPRS_PWR_PIN);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPRS_PWR_PORT, &GPIO_InitStructure);
	GPRS_PWR_PORT->ODR &= ~(GPRS_PWR_PIN);//Set GPRS PWR Pin High
	
	/* Configure  */
	GPIO_InitStructure.GPIO_Pin = (MODE_SEL1_PIN | MODE_SEL2_PIN | MODE_SEL3_PIN | MODE_SEL4_PIN);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(MODE_SEL_PORT, &GPIO_InitStructure);
}
