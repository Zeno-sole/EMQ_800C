/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/
///* Private macro -------------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/
#define ERR_INFO "Memory overflow!\r\n"

u32 count = 0;
///* Private function prototypes -----------------------------------------------*/
///* Private functions ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/


/*
*********************************************************************************************************
*	�� �� ��: HardFault_Handler
*	����˵��: Ӳ�������жϷ�����������쳣�����رգ����ַ������쳣���򴥷���
*			  ִ���쳣����ʱ���������쳣���򴥷�����λʱĬ��ʹ�ܡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HardFault_Handler(void)
{
	const char *pError = ERR_INFO;
	uint8_t i;
	uint32_t x;
	for (i = 0; i < sizeof(ERR_INFO); i++)
	{
		UART5->DR = pError[i];
		while ((UART5->SR & USART_FLAG_TC) == (uint16_t)RESET);
		for(x=0;x<0x0000ffff;x++){}
	}
	
	for(x=0;x<0x00ffffff;x++){}
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	OSIntNestingCtr++;                                      /* Tell uC/OS-III that we are starting an ISR             */
	CPU_CRITICAL_EXIT();
	OSTimeTick();                                           /* Call uC/OS-III's OSTimeTick()                          */
	OSIntExit();                                            /* Tell uC/OS-III that we are leaving the ISR             */
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
  OSIntEnter();//OSIntNestingCtr++; 
	CPU_CRITICAL_EXIT();
	BSP_GPRS_USART_IRQHandler();
	OSIntExit();
}

/**
  * @brief  This function handles USART3 global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
  OSIntEnter();//OSIntNestingCtr++; 
	CPU_CRITICAL_EXIT();
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		PM25_RxBuff[PM25_RxBuffCount++] = USART3->DR;
		if(PM25_RxBuffCount >= 256)PM25_RxBuffCount = 0;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
	OSIntExit();
}

/**
  * @brief  This function handles UART4 global interrupt request.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
  OSIntEnter();//OSIntNestingCtr++; 
	CPU_CRITICAL_EXIT();
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
		Modbus_Master_Rece_Handler();
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
	}else if(USART_GetITStatus(UART4, USART_IT_TC) != RESET){ //������ɺ��������
		RS485_RDEN_PORT->ODR &= ~(RS485_RDEN_PIN);//Low Receiver
		USART_ClearITPendingBit(UART4,USART_IT_TC);
	}
	OSIntExit();
}

/**
  * @brief  This function handles UART5 global interrupt request.
  * @param  None
  * @retval None
  */
void UART5_IRQHandler(void)
{
	u8 rec;	
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
  OSIntEnter();//OSIntNestingCtr++; 
	CPU_CRITICAL_EXIT();
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){

			rec = (u8)USART_ReceiveData(UART5);  
		
			RxCmdBuff[RxCmdCount] = rec;
			RxCmdCount++;

			if(rec == 'w')
			{
				RxCmdCount = 0;

			}
			else if(rec == 'd')
			{
				RxCmdCount = 0;
				memset(RxCmdBuff, 0, sizeof(RxCmdBuff));
//				sprintf(temp_id,"DEVICE_ID:%d\r\n",DEVICE_ID);
//			  Uart5WriteBuffer_Config((u8 *)temp_id, strlen(temp_id));
//				memset(temp_id, 0, sizeof(temp_id));
				read_DEVICE_ID();
			}
			if(RxCmdCount > 128) 
			{
				RxCmdCount = 0;
				memset(RxCmdBuff, 0, sizeof(RxCmdBuff));
			}

		}
	OSIntExit();
}
/*-------------------------------------------------*/
/*����������ʱ��3�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM3_IRQHandler(void)
{
	char temp[100]={0};
//		u8 res;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)                //���TIM3�����жϷ������
	{
		memcpy(&EMQTT_RxDataInPtr[1],SIM800C_RX_BUF,SIM800C_RX_STA);        //�������ݵ����ջ�����
		EMQTT_RxDataInPtr[0] = SIM800C_RX_STA;                           //��¼���ݳ���	
		EMQTT_RxDataInPtr+=EMQTT_UNIT;                                //ָ������
		if(EMQTT_RxDataInPtr==EMQTT_RxDataEndPtr)                     //���ָ�뵽������β����
			EMQTT_RxDataInPtr = EMQTT_RxDataBuf[0];                   //ָ���λ����������ͷ
	  neul_bc95_str_to_hex((const unsigned char *)SIM800C_RX_BUF,strlen((const char *)SIM800C_RX_BUF),temp);

		printf("���յ�����\r\n");
		UartWriteBuffer_Config((u8 *)temp,strlen((char *)temp));		
		
		if((temp[0] == 0x30) && (temp[1] == 0x44)&& (temp[2] == 0x30)&& (temp[3] == 0x41))
		{
	  printf("���½�������r\n");
			
		EMQTT_TxDataOutPtr = EMQTT_TxDataBuf[0];   //ָ���λ����������ͷ		
		memset(EMQTT_RxDataOutPtr, 0, sizeof((char *)EMQTT_RxDataOutPtr));
		memset(EMQTT_TxDataBuf, 0, sizeof((char *)EMQTT_TxDataBuf));					
//		TIM_Cmd(TIM4, DISABLE);                        				  //�ر�TIM3	
//		ResConnect();                         //����			
		EMQ_RES = 2;
		}
		
		TIM_SetCounter(TIM4, 0);                                      //�嶨ʱ��4
		TIM_Cmd(TIM3, DISABLE);                        				  //�ر�TIM3
		TIM_SetCounter(TIM3, 0);                                      //���㶨ʱ��3������	
		SIM800C_RX_STA=0;                                             //�����־
		memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);                 //���SIM800C���ڽ��ջ����� 
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                   //���TIMx�����жϱ�־ 
	}
}

/*-------------------------------------------------*/
/*����������ʱ��2�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����
D1   PM2.5       ��λug
D2   PM 10	     ��λug
D3   ����ǿ��    ��λdB
D4   ����ʪ��    ��λRH%
D5   �����¶�    ��λ��
D6   ����ѹ      ��λ Kpa
D7   ����        ��λM/S
D8   ����		 �������ݶ�Ӧ��ϵ��
D9   �缶
D10  TSP		 ��λug
D11  ��������	 ��λug
D12  VOC
D13  ��������
D14  ��ȩ 	     ��λ mg���O
D15  ����ʪ��     ��λ RH%
D16  ����ǿ��    ��λ lx
D17  ������̼Ũ��	��λ ppm*/
/*-------------------------------------------------*/
void TIM2_IRQHandler(void)
{	

	char temp_buff[800];
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
	{

		count++;
		if(count == 3)
		{
		count = 0;
		sprintf(temp_buff,"{'D1':%d,'D2':%d,'D3':%d,'D4':%4.2f,'D5':%2.2f,'D6':0,'D7':%2.1f,'D8':%d,'D9':%d,'D10':%d}",
						mySensorDatas.PM2_5,
						mySensorDatas.PM10,
						mySensorDatas.NOISE,
						(float)mySensorDatas.HUMI/100,
						(float)mySensorDatas.TEMP/100,
						(float)mySensorDatas.WIND_SPEED/10,
						mySensorDatas.WIND_DIRECTION_CLASS,
						mySensorDatas.WIND_LEVEL,
						mySensorDatas.TSP);          //�����ϴ�����		

		EMQTT_PUBLISH_Qs0(Deal_CMD,(u8 *)temp_buff);                            //�������� 
		}
//		TIM_SetCounter(TIM2, 0);     //���㶨ʱ��5������	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);                       //���TIM2�����жϱ�־ 
	}
	
}

/*-------------------------------------------------*/
/*����������ʱ��4�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM4_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���TIM4�����жϷ������
	{
		if(ping_flag==0)        //����0��ʱ�򣬷���ping����
		{
			
			printf("\r\n׼������ping����\r\n");
			EMQTT_PINGREQ();             //ping����
			TIM_SetCounter(TIM5, 0);     //���㶨ʱ��5������	
			TIM_Cmd(TIM5, ENABLE);       //������ʱ��5
			ping_flag=1;	             //�յ��������ظ�������ñ�־			
		}

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);    //���TIM4�����жϱ�־ 
	}
}

/*-------------------------------------------------*/
/*����������ʱ��5�жϷ�����                      */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM5_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //���TIM5�����жϷ������
	{


	
		printf("\r\nping��ʱ,��Ҫ����\r\n");
		serverLink_sta = 0;
		TIM_SetCounter(TIM5, 0);     //���㶨ʱ��5������	
		TIM_SetCounter(TIM4, 0);     //���㶨ʱ��5������	
		TIM_Cmd(TIM4, DISABLE);    
//		ResConnect();
		
		NVIC_SystemReset();                            //����
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);    //���TIM5�����жϱ�־ 
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
