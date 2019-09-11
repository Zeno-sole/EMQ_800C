#ifndef  BSP_GPIO_
#define  BSP_GPIO_

#include "stm32f10x.h"

/*---------------------------- RS485 RDEN define -------------------------*/
#define RS485_RDEN_PIN                 (uint16_t)GPIO_Pin_15  //OK
#define RS485_RDEN_PORT                 GPIOA
/*---------------------------- LINK LED define ---------------------------*/
#define LINK_LED_PIN                   (uint16_t)GPIO_Pin_9   //OK
#define LINK_LED_PORT                   GPIOD
/*----------------------------- Relay define -----------------------------*/
#define RELAY_PIN                      (uint16_t)GPIO_Pin_0   //OK
#define RELAY_PORT                      GPIOE
/*---------------------------- GPRS PWR define ---------------------------*/
#define GPRS_PWR_PIN                   (uint16_t)GPIO_Pin_3  //Œ¥ π”√
#define GPRS_PWR_PORT                   GPIOE
/*-------------------------- Mode Selection define -----------------------*/
#define MODE_SEL1_PIN                  (uint16_t)GPIO_Pin_15  //OK
#define MODE_SEL2_PIN                  (uint16_t)GPIO_Pin_14  //OK
#define MODE_SEL3_PIN                  (uint16_t)GPIO_Pin_13  //OK
#define MODE_SEL4_PIN                  (uint16_t)GPIO_Pin_12  //OK
#define MODE_SEL_PORT                   GPIOB
/*------------------------------------------------------------------------*/

void BSP_GPIO_Init(void);

#endif /* BSP_GPIO_ */

