/*-------------------------------------------------*/
/*-------------------------------------------------*/

#include "timer4.h"

/*-------------------------------------------------*/
/*����������ʱ��4��ʼ��   30s                      */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM4_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM_Cmd(TIM4,DISABLE);                              //�ض�ʱ��22
	TIM_SetCounter(TIM4, 0);                            //�嶨ʱ��	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);           //ʹ��TIM4ʱ��	
  TIM_TimeBaseInitStructure.TIM_Period = 60000-1; 	          //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=54000-1;              //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);            //����������TIM4
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);                    //����жϱ�־λ
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                      //����ʱ��4�����ж�    
	TIM_Cmd(TIM4,ENABLE);                                         

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;                 //��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;    //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;           //�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                               //���������� 
}


